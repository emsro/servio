#pragma once

#include "drv/callbacks.hpp"
#include "drv/interfaces.hpp"
#include "drv/retainers.hpp"
#include "ftest/utest.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>

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
struct clock_test
{
        clk_iface& clk;

        std::string_view name = "clock_test";

        t::coroutine< void > run( auto&, ftest::uctx& ctx )
        {
                std::size_t  wait_cycles = 100;
                microseconds t1          = clk.get_us();
                for ( std::size_t i = 0; i < wait_cycles; i++ )
                        asm( "nop" );
                microseconds t2 = clk.get_us();
                co_await ctx.expect( t2 > t1 );
                ctx.coll.set( 0, "t1", t1 );
                ctx.coll.set( 0, "t2", t2 );
                ctx.coll.set( 0, "wait cycles", wait_cycles );

                microseconds now  = clk.get_us();
                microseconds last = now;
                microseconds end  = now + 2_s;
                do {
                        last = now;
                        now  = clk.get_us();
                        if ( last > now ) {
                                ctx.coll.set( "last", last );
                                ctx.coll.set( "now", now );
                        }
                        co_await ctx.expect( last <= now );
                } while ( clk.get_us() < end );
        }
};

// Test comms:
//  - send a message and wait for reply
//  - assumes that comms is connected in a way that echo goes back
struct comms_echo_test
{
        clk_iface& clk;
        com_iface& comms;

        std::string_view name = "comms_echo";

        t::coroutine< void > run( auto& mem, ftest::uctx& ctx )
        {
                std::array< std::byte, 6 > buffer{ 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b };
                em::result                 res = send( comms, 100_ms, buffer );
                co_await ctx.expect( res == em::SUCCESS );

                wait_for( clk, 10_ms );

                std::array< std::byte, 6 > buffer2;
                bool                       ready = false;
                em::view< std::byte* >     v;
                while ( !ready )
                        std::tie( ready, v ) = comms.recv( buffer2 );

                store_data( mem, ctx, "data", v );

                co_await ctx.expect( v == em::view{ buffer } );
        }
};

// Test comms:
//  - send message and immediatly send another one - triggering timeout
//  - check that timeout was reported - operation errored
//  - assumes that comms is connected in a way that echo goes back
//  - send another message a while after it
struct comms_timeout_test
{
        clk_iface& clk;
        com_iface& comms;

        std::string_view name = "comms_timeout";

        t::coroutine< void > run( auto& mem, ftest::uctx& ctx )
        {
                std::array< std::byte, 6 > buffer{ 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b };
                em::result                 res = send( comms, 1_us, buffer );
                co_await ctx.expect( res == em::SUCCESS );

                res = send( comms, 1_us, buffer );
                co_await ctx.expect( res == em::ERROR );

                wait_for( clk, 10_ms );

                res = send( comms, 100_ms, buffer );
                co_await ctx.expect( res != em::ERROR );

                wait_for( clk, 10_ms );

                std::array< std::byte, 6 > buffer2;
                bool                       ready = false;
                em::view< std::byte* >     v;
                while ( !ready )
                        std::tie( ready, v ) = comms.recv( buffer2 );

                store_data( mem, ctx, "data", v );

                co_await ctx.expect( v == em::view{ buffer } );
        }
};

using namespace std::literals;

// Tests for periodic interface
//  - stop the interface
//  - install callback and check that it was installed
//  - check that it is not called
//  - start the interface
//  - check that callback gets called
struct period_iface_test
{
        clk_iface&    clk;
        period_iface& iface;

        std::string_view name = "period_iface";

        t::coroutine< void > run( auto& mem, ftest::uctx& ctx )
        {
                auto d = retain_callback( iface );
                co_await ctx.expect( iface.stop() == em::SUCCESS );

                std::size_t counter = 0;
                period_cb   pcb{ [&] {
                        counter += 1;
                } };
                iface.set_period_callback( pcb );
                co_await ctx.expect( &iface.get_period_callback() == &pcb );

                wait_for( clk, 10_ms );
                co_await ctx.expect( counter == 0 );
                ctx.coll.set( "counter1", counter );

                co_await ctx.expect( iface.start() == em::SUCCESS );

                wait_for( clk, 10_ms );

                co_await store_metric( mem, ctx, "counter", counter );
                co_await ctx.expect( &iface.get_period_callback() == &pcb );
                co_await ctx.expect( counter != 0 );
        }
};

// Test PWM motor interface
//  - just checks whenever direction matches set power
// note: force_stop not tested, which is kinda mistake but it's also hard to test :)
struct pwm_motor_test
{
        period_iface&    period;
        pwm_motor_iface& iface;

        std::string_view name = "pwm_motor";

        auto test(
            ftest::uctx&         ctx,
            pwr                  p,
            int16_t              expected,
            std::source_location src = std::source_location::current() )
        {
                iface.set_power( p );
                ctx.coll.set( "last_dir", iface.get_direction() );
                ctx.coll.set( "expected", expected );
                return ctx.expect( iface.get_direction() == expected, src );
        }

        t::coroutine< void > run( auto&, ftest::uctx& ctx )
        {
                co_await ctx.expect( !iface.is_stopped() );
                auto d = hold( period );

                co_await test( ctx, pwr{ -0.5 }, -1 );
                co_await test( ctx, p_zero, 1 );
                co_await test( ctx, pwr{ 0.5 }, 1 );
        }
};

// Test vcc reading
//  - just check that voltage is nonzero
struct vcc_test
{
        vcc_iface& iface;

        std::string_view name = "vcc_test";

        t::coroutine< void > run( auto& mem, ftest::uctx& ctx )
        {
                uint32_t vcc = iface.get_vcc();

                co_await store_metric( mem, ctx, "vcc", vcc );
                co_await ctx.expect( vcc != 0 );
        }
};

// Test temperature reading
//  - just check that temp is nonzero
struct temperature_test
{
        temp_iface& iface;

        std::string_view name = "temp_test";

        t::coroutine< void > run( auto& mem, ftest::uctx& ctx )
        {
                int32_t temp = iface.get_temperature();

                co_await store_metric( mem, ctx, "temp", temp );
                co_await ctx.expect( temp != 0 );
        }
};

// Test position iface
//  - check that callback can be changed properly
//  - check that callback was called
struct position_test
{
        pos_iface& iface;
        clk_iface& clk;

        std::string_view name = "pos_test";

        t::coroutine< void > run( auto& mem, ftest::uctx& ctx )
        {

                em::defer d = retain_callback( iface );

                std::optional< uint32_t > opt_pos = std::nullopt;
                position_cb               pcb{ [&]( uint32_t pos ) {
                        opt_pos = pos;
                } };
                iface.set_position_callback( pcb );
                co_await ctx.expect( &iface.get_position_callback() == &pcb );

                wait_for( clk, 10_ms );

                co_await ctx.expect( opt_pos.has_value() );
                co_await store_metric( mem, ctx, "pos", opt_pos.value() );
        }
};

// Test current iface
//  - check that callback can be changed properly and that it is called
struct curr_iface_test
{
        curr_iface& iface;
        clk_iface&  clk;

        std::string_view name = "curr_test";

        t::coroutine< void > run( auto& mem, ftest::uctx& ctx )
        {
                em::defer d = retain_callback( iface );

                std::optional< uint32_t > opt_curr = std::nullopt;
                std::span< uint16_t >     prof;
                current_cb                ccb{ [&]( uint32_t curr, std::span< uint16_t > profile ) {
                        opt_curr = curr;
                        prof     = profile;
                } };
                iface.set_current_callback( ccb );
                co_await ctx.expect( &iface.get_current_callback() == &ccb );

                wait_for( clk, 10_ms );

                co_await store_metric( mem, ctx, "prof size", prof.size() );
                co_await ctx.expect( !prof.empty() );

                co_await ctx.expect( opt_curr.has_value() );
                co_await store_metric( mem, ctx, "curr", opt_curr.value() );
        }
};

inline void setup_iface_tests(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    ftest::uctx&              ctx,
    clk_iface&                clk,
    com_iface&                comms,
    period_iface&             period,
    pwm_motor_iface&          pwm,
    vcc_iface&                vcc,
    temp_iface&               temp,
    pos_iface&                pos,
    curr_iface&               curr,
    em::result&               res )
{
        ftest::setup_utest< clock_test >( mem, reac, ctx, res, clk );
        ftest::setup_utest< comms_echo_test >( mem, reac, ctx, res, clk, comms );
        ftest::setup_utest< comms_timeout_test >( mem, reac, ctx, res, clk, comms );
        ftest::setup_utest< period_iface_test >( mem, reac, ctx, res, clk, period );
        ftest::setup_utest< pwm_motor_test >( mem, reac, ctx, res, period, pwm );
        ftest::setup_utest< vcc_test >( mem, reac, ctx, res, vcc );
        ftest::setup_utest< temperature_test >( mem, reac, ctx, res, temp );
        ftest::setup_utest< position_test >( mem, reac, ctx, res, pos, clk );
        ftest::setup_utest< curr_iface_test >( mem, reac, ctx, res, curr, clk );
}

}  // namespace servio::drv::tests
