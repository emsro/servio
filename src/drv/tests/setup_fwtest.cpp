#include "drv/tests/setup_fwtest.hpp"

#include "base/callbacks.hpp"
#include "drv/cobs_uart.hpp"
#include "drv/hbridge.hpp"
#include "drv/retainers.hpp"
#include "sntr/central_sentry_iface.hpp"

namespace servio::drv::tests
{

using namespace base::literals;

template < typename T >
struct utest : em::testing::test_interface
{
        T item;

        template < typename... Args >
        utest( Args&&... args )
          : item( std::forward< Args >( args )... )
        {
        }

        std::string_view get_name() const override
        {
                return item.name;
        }

        em::testing::test_coroutine run( em::pmr::memory_resource& mem, em::testing::record& rec )
        {
                return item.run( mem, rec );
        }
};

template < typename T, typename... Args >
void setup_utest(
    em::pmr::memory_resource& mem,
    em::testing::reactor&     reac,
    em::result&               res,
    Args&&... args )
{
        if ( res != em::SUCCESS )
                return;
        res = em::testing::construct_test_unit< utest< T > >(
            mem, reac, std::forward< Args >( args )... );
}

struct clock_test
{
        em::testing::collector& coll;
        base::clk_interface&    clk;

        std::string_view name = "clock_test";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                std::size_t        wait_cycles = 10000;
                base::microseconds t1          = clk.get_us();
                for ( std::size_t i = 0; i < wait_cycles; i++ )
                        asm( "nop" );
                base::microseconds t2 = clk.get_us();
                rec.expect( t2 > t1 );
                coll.set( 0, "t1", t1 );
                coll.set( 0, "t2", t2 );
                coll.set( 0, "wait cycles", wait_cycles );

                co_return;
        }
};

struct comms_echo_test
{
        base::com_interface& comms;
        std::string_view     name = "comms_echo";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                std::array< std::byte, 6 > buffer{ 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b };
                em::result                 res = comms.send( buffer, 100_ms );
                rec.expect( res == em::SUCCESS );

                std::array< std::byte, 6 > buffer2;
                auto [load_res, used_data] = comms.load_message( buffer2 );
                rec.expect( load_res );
                rec.expect( used_data == em::view{ buffer } );
                co_return;
        }
};

struct comms_timeout_test
{
        base::com_interface& comms;
        std::string_view     name = "comms_timeout";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                std::array< std::byte, 6 > buffer{ 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b };
                em::result                 res = comms.send( buffer, 1_us );
                rec.expect( res == em::SUCCESS );

                res = comms.send( buffer, 1_us );
                rec.expect( res == em::ERROR );

                bool                   ready = false;
                em::view< std::byte* > v;
                while ( !ready )
                        std::tie( ready, v ) = comms.load_message( buffer );

                res = comms.send( buffer, 100_ms );
                co_await rec.expect( res != em::ERROR );
        }
};

struct test_central_sentry : sntr::central_sentry_iface
{
        enum src
        {
                INOP,
                DEGR
        };

        using record =
            std::tuple< src, const char*, sntr::ecode_set, const char*, sntr::data_type >;
        em::static_vector< record, 8 > buffer;
        bool                           is_inop = false;

        bool is_inoperable() const override
        {
                return is_inop;
        }

        void report_inoperable(
            const char*            src,
            sntr::ecode_set        ecodes,
            const char*            emsg,
            const sntr::data_type& data ) override
        {
                is_inop = true;
                if ( !buffer.full() )
                        buffer.emplace_back( INOP, src, ecodes, emsg, data );
        }

        void report_degraded(
            const char*            src,
            sntr::ecode_set        ecodes,
            const char*            emsg,
            const sntr::data_type& data ) override
        {
                if ( !buffer.full() )
                        buffer.emplace_back( DEGR, src, ecodes, emsg, data );
        }
};

struct cobs_uart_rx_test
{
        base::clk_interface& clk;
        std::string_view     name = "cobs_uart_rx";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                test_central_sentry central;

                cobs_uart uart{ "test_uart", central, clk, nullptr, nullptr };

                rec.expect( central.buffer.empty() );
                uart.rx_cplt_irq( nullptr );
                rec.expect( !central.buffer.empty() );
                rec.expect( central.is_inoperable() );

                co_return;
        }
};

struct cobs_uart_err_test
{
        base::clk_interface& clk;
        std::string_view     name = "cobs_uart_err";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                test_central_sentry central;
                UART_HandleTypeDef  handle;
                cobs_uart           uart{ "test_uart", central, clk, &handle, nullptr };

                rec.expect( central.buffer.empty() );
                handle.ErrorCode = cobs_uart::tolerable_hal_errors;
                uart.err_irq( &handle );
                if ( central.buffer.size() != 1 ) {
                        rec.fail();
                        co_return;
                }
                rec.expect( !central.is_inoperable() );
                rec.expect( std::get< 0 >( central.buffer[0] ) == test_central_sentry::DEGR );

                handle.ErrorCode = ~cobs_uart::tolerable_hal_errors;
                uart.err_irq( &handle );
                co_await rec.expect( central.buffer.size() != 2 );

                rec.expect( central.is_inoperable() );
                rec.expect( std::get< 0 >( central.buffer[1] ) == test_central_sentry::INOP );

                co_return;
        }
};

struct period_iface_test
{
        base::clk_interface&    clk;
        base::period_interface& iface;
        std::string_view        name = "period_iface";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                auto d = retain_callback( iface );
                co_await rec.expect( iface.stop() == em::SUCCESS );

                std::size_t     counter = 0;
                base::period_cb pcb{ [&] {
                        counter += 1;
                } };
                iface.set_period_callback( pcb );
                co_await rec.expect( &iface.get_period_callback() == &pcb );

                base::wait_for( clk, 10_ms );
                rec.expect( counter == 0 );

                co_await rec.expect( iface.start() != em::SUCCESS );
                base::wait_for( clk, 10_ms );
                rec.expect( counter != 0 );
        }
};

struct pwm_motor_test
{
        base::pwm_motor_interface& iface;
        std::string_view           name = "pwm_motor";

        // note: force_stop not tested, which is kinda mistake but it's also hard to test :)
        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                test( rec, -1, -1 );
                test( rec, 0, 1 );
                test( rec, 0, 1 );
                co_return;
        }

        void test( em::testing::record& rec, int16_t pwr, int16_t expected )
        {
                iface.set_power( pwr );
                rec.expect( iface.get_direction() == expected );
        }
};

struct hbridge_test
{
        std::string_view name = "hbridge_test";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                hbridge hb{ nullptr };
                rec.expect( hb.setup( 1, 2 ) == nullptr );

                hb.set_power( -1 );
                rec.expect( hb.get_direction() == 0 );

                auto d = retain_callback( hb );

                std::size_t     counter = 0;
                base::period_cb pcb{ [&] {
                        counter += 1;
                } };
                hb.set_period_callback( pcb );
                rec.expect( &hb.get_period_callback() == &pcb );

                co_await rec.expect( counter == 0 );
                hb.timer_period_irq( nullptr );
                co_await rec.expect( counter == 1 );

                rec.expect( hb.start() == em::ERROR );
                rec.expect( hb.stop() == em::ERROR );
        }
};

// TODO: write this!
struct leds_test
{
};

// also skipped, mindfuck
struct adc_pooler_test
{
};

struct vcc_test
{
        base::vcc_interface&    iface;
        em::testing::collector& coll;
        std::string_view        name = "vcc_test";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                auto vcc = iface.get_vcc();
                coll.set( 0, "vcc", vcc );
                rec.expect( vcc != 0 );

                co_return;
        }
};

struct temperature_test
{
        base::temperature_interface& iface;
        em::testing::collector&      coll;
        std::string_view             name = "temp_test";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                auto temp = iface.get_temperature();
                coll.set( 0, "temp", temp );
                rec.expect( temp != 0 );

                co_return;
        }
};

struct position_test
{
        base::position_interface& iface;
        base::clk_interface&      clk;
        em::testing::collector&   coll;
        std::string_view          name = "pos_test";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                em::defer d = retain_callback( iface );

                std::optional< uint32_t > opt_pos = std::nullopt;
                base::position_cb         pcb{ [&]( uint32_t pos ) {
                        opt_pos = pos;
                } };
                iface.set_position_callback( pcb );
                co_await rec.expect( &iface.get_position_callback() == &pcb );

                base::wait_for( clk, 10_ms );

                rec.expect( opt_pos.has_value() );
        }
};

void setup_tests(
    em::pmr::memory_resource&    mem,
    em::testing::reactor&        reac,
    em::testing::collector&      coll,
    base::clk_interface&         clk,
    base::com_interface&         comms,
    base::period_interface&      period,
    base::pwm_motor_interface&   pwm,
    base::vcc_interface&         vcc,
    base::temperature_interface& temp,
    base::position_interface&    pos,
    em::result&                  res )

{
        setup_utest< clock_test >( mem, reac, res, coll, clk );
        setup_utest< comms_echo_test >( mem, reac, res, comms );
        setup_utest< comms_timeout_test >( mem, reac, res, comms );
        setup_utest< cobs_uart_rx_test >( mem, reac, res, clk );
        setup_utest< cobs_uart_err_test >( mem, reac, res, clk );
        setup_utest< period_iface_test >( mem, reac, res, clk, period );
        setup_utest< pwm_motor_test >( mem, reac, res, pwm );
        setup_utest< hbridge_test >( mem, reac, res );
        setup_utest< vcc_test >( mem, reac, res, vcc, coll );
        setup_utest< temperature_test >( mem, reac, res, temp, coll );
        setup_utest< position_test >( mem, reac, res, pos, clk, coll );
}

}  // namespace servio::drv::tests
