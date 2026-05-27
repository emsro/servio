#pragma once

#include "../../ftest/utest.hpp"
#include "../callbacks.hpp"
#include "../interfaces.hpp"
#include "../retainers.hpp"

namespace em = emlabcpp;

namespace servio::drv::tests
{

inline auto hold( auto& iface )
{
        std::ignore = iface.stop();
        return em::defer{ [&] {
                std::ignore = iface.start();
        } };
}

// Test that clock of chip works correctly:
//  - time increases
struct clock_test : ftest::utest
{
        clk_iface& clk;

        char const* name = "clock_test";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                std::size_t  wait_cycles = 100;
                microseconds t1          = clk.get_us();
                for ( std::size_t i = 0; i < wait_cycles; i++ )
                        asm( "nop" );
                microseconds t2 = clk.get_us();
                co_await expect( *this, t2 > t1 );
                co_await store_metric( *this, "t1", t1 );
                co_await store_metric( *this, "t2", t2 );
                co_await store_metric( *this, "wait_cycles", wait_cycles );

                microseconds now  = clk.get_us();
                microseconds last = now;
                microseconds end  = now + 2_s;
                do {
                        last = now;
                        now  = clk.get_us();
                        if ( last > now ) {
                                co_await store_metric( *this, "last", last );
                                co_await store_metric( *this, "now", now );
                        }
                        co_await expect( *this, last <= now );
                } while ( clk.get_us() < end );
        }
};

// Test comms:
//  - send a message and wait for reply
//  - assumes that comms is connected in a way that echo goes back
struct comms_echo_test : ftest::utest
{
        clk_iface& clk;
        com_iface& comms;

        char const* name = "comms_echo";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                std::array< std::byte, 6 > buffer{ 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b };
                auto                       res = send( comms, 100_ms, buffer );
                co_await expect( *this, res == status::success );

                wait_for( clk, 10_ms );

                std::array< std::byte, 7 > buffer2{
                    0x66_b, 0x66_b, 0x66_b, 0x66_b, 0x66_b, 0x66_b, 0x66_b };
                bool                   ready = false;
                em::view< std::byte* > v;
                while ( !ready )
                        std::tie( ready, v ) = comms.recv( buffer2 );

                co_await expect( *this, v == em::view{ buffer } );
        }
};

// Test comms:
//  - send message and immediatly send another one - triggering timeout
//  - check that timeout was reported - operation errored
//  - assumes that comms is connected in a way that echo goes back
//  - send another message a while after it
struct comms_timeout_test : ftest::utest
{
        clk_iface& clk;
        com_iface& comms;

        char const* name = "comms_timeout";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                std::array< std::byte, 6 > buffer{ 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b };
                auto                       res = send( comms, 1_us, buffer );
                co_await expect( *this, res == status::success );

                res = send( comms, 1_us, buffer );
                co_await expect( *this, res == status::error );

                wait_for( clk, 10_ms );

                res = send( comms, 100_ms, buffer );
                co_await expect( *this, res != status::error );

                wait_for( clk, 10_ms );

                std::array< std::byte, 6 > buffer2;
                bool                       ready = false;
                em::view< std::byte* >     v;
                while ( !ready )
                        std::tie( ready, v ) = comms.recv( buffer2 );

                co_await expect( *this, v == em::view{ buffer } );
        }
};

using namespace std::literals;

// Tests for periodic interface
//  - stop the interface
//  - install callback and check that it was installed
//  - check that it is not called
//  - start the interface
//  - check that callback gets called
struct period_iface_test : ftest::utest
{
        clk_iface&    clk;
        period_iface& iface;

        char const* name = "period_iface";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                auto d = retain_callback( iface );
                co_await expect( *this, iface.stop() == status::success );

                std::size_t counter = 0;
                period_cb   pcb{ [&] {
                        counter += 1;
                } };
                iface.set_period_callback( pcb );
                co_await expect( *this, &iface.get_period_callback() == &pcb );

                wait_for( clk, 10_ms );
                co_await expect( *this, counter == 0 );
                co_await store_metric( *this, "counter1", counter );

                co_await expect( *this, iface.start() == status::success );

                wait_for( clk, 10_ms );

                co_await store_metric( *this, "counter", counter );
                co_await expect( *this, &iface.get_period_callback() == &pcb );
                co_await expect( *this, counter != 0 );
        }
};

// Test PWM motor interface
//  - just checks whenever direction matches set power
// note: force_stop not tested, which is kinda mistake but it's also hard to test :)
struct pwm_motor_test : ftest::utest
{
        period_iface&    period;
        pwm_motor_iface& iface;

        char const* name = "pwm_motor";

        ftest::task< void >
        test( pwr p, int16_t expected, std::source_location src = std::source_location::current() )
        {
                iface.set_power( p );
                co_await store_metric( *this, "last_dir", iface.get_direction() );
                co_await store_metric( *this, "expected", expected );
                co_await expect( *this, iface.get_direction() == expected, src );
        }

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                co_await expect( *this, !iface.is_stopped() );
                auto d = hold( period );

                co_await test( -0.5_pwr, -1 );
                co_await test( 0_pwr, 1 );
                co_await test( 0.5_pwr, 1 );
        }
};

// Test vcc reading
//  - just check that voltage is nonzero
struct vcc_test : ftest::utest
{
        vcc_iface& iface;

        char const* name = "vcc_test";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                uint32_t vcc = iface.get_vcc();

                co_await store_metric( *this, "vcc", vcc );
                co_await expect( *this, vcc != 0 );
        }
};

// Test temperature reading
//  - just check that temp is nonzero
struct temperature_test : ftest::utest
{
        temp_iface& iface;

        char const* name = "temp_test";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                int32_t temp = iface.get_temperature();

                co_await store_metric( *this, "temp", temp );
                co_await expect( *this, temp != 0 );
        }
};

// Test position iface
//  - check that callback can be changed properly
//  - check that callback was called
struct position_test : ftest::utest
{
        pos_iface& iface;
        clk_iface& clk;

        char const* name = "pos_test";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = retain_callback( iface );

                opt< uint32_t > opt_pos = std::nullopt;
                position_cb     pcb{ [&]( uint32_t pos ) {
                        opt_pos = pos;
                } };
                iface.set_position_callback( pcb );
                co_await expect( *this, &iface.get_position_callback() == &pcb );

                wait_for( clk, 10_ms );

                co_await expect( *this, opt_pos.has_value() );
                co_await store_metric( *this, "pos", opt_pos.value() );
        }
};

// Test current iface
//  - check that callback can be changed properly and that it is called
struct curr_iface_test : ftest::utest
{
        curr_iface& iface;
        clk_iface&  clk;

        char const* name = "curr_test";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = retain_callback( iface );

                opt< uint32_t >       opt_curr = std::nullopt;
                std::span< uint16_t > prof;
                current_cb            ccb{ [&]( uint32_t curr, std::span< uint16_t > profile ) {
                        opt_curr = curr;
                        prof     = profile;
                } };
                iface.set_current_callback( ccb );
                co_await expect( *this, &iface.get_current_callback() == &ccb );

                wait_for( clk, 10_ms );

                co_await store_metric( *this, "prof size", prof.size() );
                co_await expect( *this, !prof.empty() );

                co_await expect( *this, opt_curr.has_value() );
                co_await store_metric( *this, "curr", opt_curr.value() );
        }
};

struct storage_iface_test : ftest::utest
{
        cfg::map&      cfg;
        storage_iface& iface;

        char const* name = "storage_iface";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                // XXX: this could be improved a lot
                cfg::map m;
                co_await expect( *this, iface.load_cfg( m ) == status::success );
                co_await expect( *this, iface.store_cfg( m ) == status::success );
                co_await expect( *this, iface.clear_cfg() == status::success );
                // Restore before-test settings
                co_await expect( *this, iface.store_cfg( cfg ) == status::success );
        }
};

inline void setup_iface_tests(
    asrt::task_ctx&  ctx,
    asrt_reac_assm&  assm,
    cfg::map&        cfg,
    clk_iface&       clk,
    com_iface&       comms,
    period_iface&    period,
    pwm_motor_iface& pwm,
    vcc_iface&       vcc,
    temp_iface&      temp,
    pos_iface&       pos,
    curr_iface&      curr,
    storage_iface&   siface,
    status&          res )
{
        ftest::setup_utest( ctx, assm, res, clock_test{ { ctx, assm }, clk } );
        ftest::setup_utest( ctx, assm, res, comms_echo_test{ { ctx, assm }, clk, comms } );
        ftest::setup_utest( ctx, assm, res, comms_timeout_test{ { ctx, assm }, clk, comms } );
        ftest::setup_utest( ctx, assm, res, period_iface_test{ { ctx, assm }, clk, period } );
        ftest::setup_utest( ctx, assm, res, pwm_motor_test{ { ctx, assm }, period, pwm } );
        ftest::setup_utest( ctx, assm, res, vcc_test{ { ctx, assm }, vcc } );
        ftest::setup_utest( ctx, assm, res, temperature_test{ { ctx, assm }, temp } );
        ftest::setup_utest( ctx, assm, res, position_test{ { ctx, assm }, pos, clk } );
        ftest::setup_utest( ctx, assm, res, curr_iface_test{ { ctx, assm }, curr, clk } );
        ftest::setup_utest( ctx, assm, res, storage_iface_test{ { ctx, assm }, cfg, siface } );
}

}  // namespace servio::drv::tests
