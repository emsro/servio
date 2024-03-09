#include "drv/tests/setup_fwtest.hpp"

#include "drv/cobs_uart.hpp"
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
                rec.expect( res != em::ERROR );

                co_return;
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
                if ( central.buffer.size() != 2 ) {
                        rec.fail();
                        co_return;
                }
                rec.expect( central.is_inoperable() );
                rec.expect( std::get< 0 >( central.buffer[1] ) == test_central_sentry::INOP );

                co_return;
        }
};

void setup_tests(
    em::pmr::memory_resource& mem,
    em::testing::reactor&     reac,
    em::testing::collector&   coll,
    base::clk_interface&      clk,
    base::com_interface&      comms,
    em::result&               res )

{
        setup_utest< clock_test >( mem, reac, res, coll, clk );
        setup_utest< comms_echo_test >( mem, reac, res, comms );
        setup_utest< comms_timeout_test >( mem, reac, res, comms );
        setup_utest< cobs_uart_rx_test >( mem, reac, res, clk );
        setup_utest< cobs_uart_err_test >( mem, reac, res, clk );
}

}  // namespace servio::drv::tests
