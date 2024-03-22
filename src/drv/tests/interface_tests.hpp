#pragma once

#include "drv/callbacks.hpp"
#include "drv/interfaces.hpp"
#include "drv/retainers.hpp"
#include "drv/tests/utest.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>

namespace em = emlabcpp;

namespace servio::drv::tests
{

using namespace base::literals;

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
        clk_interface& clk;

        std::string_view name = "clock_test";

        t::coroutine< void > run( auto&, collector& coll )
        {
                std::size_t        wait_cycles = 100;
                base::microseconds t1          = clk.get_us();
                for ( std::size_t i = 0; i < wait_cycles; i++ )
                        asm( "nop" );
                base::microseconds t2 = clk.get_us();
                co_await t::expect( coll, t2 > t1 );
                coll->set( 0, "t1", t1 );
                coll->set( 0, "t2", t2 );
                coll->set( 0, "wait cycles", wait_cycles );

                base::microseconds now  = clk.get_us();
                base::microseconds last = now;
                base::microseconds end  = now + 2_s;
                do {
                        last = now;
                        now  = clk.get_us();
                        if ( last > now ) {
                                coll->set( "last", last );
                                coll->set( "now", now );
                        }
                        co_await t::expect( last <= now );
                } while ( clk.get_us() < end );
        }
};

// Test comms:
//  - send a message and wait for reply
//  - assumes that comms is connected in a way that echo goes back
struct comms_echo_test
{
        clk_interface&   clk;
        com_interface&   comms;
        std::string_view name = "comms_echo";

        t::coroutine< void > run( auto&, collector& coll )
        {
                std::array< std::byte, 6 > buffer{ 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b };
                em::result                 res = comms.send( buffer, 100_ms );
                co_await t::expect( coll, res == em::SUCCESS );

                wait_for( clk, 100_ms );

                std::array< std::byte, 6 > buffer2;
                bool                       ready = false;
                em::view< std::byte* >     v;
                while ( !ready || v.size() == 0 )  // BLOCKING
                        std::tie( ready, v ) = comms.load_message( buffer2 );
                auto id = co_await coll->set( "data", em::contiguous_container_type::ARRAY );
                for ( std::byte b : v )
                        coll->append( id, static_cast< uint32_t >( b ) );
                co_await t::expect( coll, v == em::view{ buffer } );
        }
};

// Test comms:
//  - send message and immediatly send another one - triggering timeout
//  - check that timeout was reported - operation errored
//  - assumes that comms is connected in a way that echo goes back
//  - send another message a while after it
struct comms_timeout_test
{
        com_interface&   comms;
        std::string_view name = "comms_timeout";

        t::coroutine< void > run( auto&, collector& coll )
        {
                std::array< std::byte, 6 > buffer{ 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b };
                em::result                 res = comms.send( buffer, 1_us );
                co_await t::expect( coll, res == em::SUCCESS );

                res = comms.send( buffer, 1_us );
                co_await t::expect( coll, res == em::ERROR );

                bool                   ready = false;
                em::view< std::byte* > v;
                while ( !ready || v.size() == 0 )  // BLOCKING
                        std::tie( ready, v ) = comms.load_message( buffer );

                res = comms.send( buffer, 100_ms );
                co_await t::expect( coll, res != em::ERROR );
                auto id = co_await coll->set( "data", em::contiguous_container_type::ARRAY );
                for ( std::byte b : v )
                        coll->append( id, static_cast< uint32_t >( b ) );
                co_await t::expect( coll, v == em::view{ buffer } );
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
        clk_interface&    clk;
        period_interface& iface;
        std::string_view  name = "period_iface";

        t::coroutine< void > run( auto& mem, collector& coll )
        {
                auto d = retain_callback( iface );
                co_await t::expect( coll, iface.stop() == em::SUCCESS );

                std::size_t counter = 0;
                period_cb   pcb{ [&] {
                        counter += 1;
                } };
                iface.set_period_callback( pcb );
                co_await t::expect( coll, &iface.get_period_callback() == &pcb );

                wait_for( clk, 10_ms );
                co_await t::expect( coll, counter == 0 );
                coll->set( "counter1", counter );

                co_await t::expect( coll, iface.start() == em::SUCCESS );

                wait_for( clk, 10_ms );

                co_await store_metric( mem, coll, "counter", counter );
                co_await t::expect( coll, &iface.get_period_callback() == &pcb );
                co_await t::expect( coll, counter != 0 );
        }
};

// Test PWM motor interface
//  - just checks whenever direction matches set power
// note: force_stop not tested, which is kinda mistake but it's also hard to test :)
struct pwm_motor_test
{
        period_interface&    period;
        pwm_motor_interface& iface;
        std::string_view     name = "pwm_motor";

        auto test(
            collector&           coll,
            int16_t              pwr,
            int16_t              expected,
            std::source_location src = std::source_location::current() )
        {
                iface.set_power( pwr );
                coll->set( "last_dir", iface.get_direction() );
                return t::expect( coll, iface.get_direction() == expected, src );
        }

        t::coroutine< void > run( auto&, collector& coll )
        {
                co_await t::expect( coll, !iface.is_stopped() );
                auto d = hold( period );

                co_await test( coll, -1024, -1 );
                co_await test( coll, 0, 1 );
                co_await test( coll, 1024, 1 );
        }
};

// Test vcc reading
//  - just check that voltage is nonzero
struct vcc_test
{
        vcc_interface&   iface;
        std::string_view name = "vcc_test";

        t::coroutine< void > run( auto& mem, collector& coll )
        {
                uint32_t vcc = iface.get_vcc();

                co_await store_metric( mem, coll, "vcc", vcc );
                co_await t::expect( coll, vcc != 0 );
        }
};

// Test temperature reading
//  - just check that temp is nonzero
struct temperature_test
{
        temperature_interface& iface;
        std::string_view       name = "temp_test";

        t::coroutine< void > run( auto& mem, collector& coll )
        {
                uint32_t temp = iface.get_temperature();

                co_await store_metric( mem, coll, "temp", temp );
                co_await t::expect( coll, temp != 0 );
        }
};

// Test position iface
//  - check that callback can be changed properly
//  - check that callback was called
struct position_test
{
        position_interface& iface;
        clk_interface&      clk;
        std::string_view    name = "pos_test";

        t::coroutine< void > run( auto& mem, collector& coll )
        {

                em::defer d = retain_callback( iface );

                std::optional< uint32_t > opt_pos = std::nullopt;
                position_cb               pcb{ [&]( uint32_t pos ) {
                        opt_pos = pos;
                } };
                iface.set_position_callback( pcb );
                co_await t::expect( coll, &iface.get_position_callback() == &pcb );

                wait_for( clk, 10_ms );

                co_await t::expect( coll, opt_pos.has_value() );
                co_await store_metric( mem, coll, "pos", opt_pos.value() );
        }
};

// Test current iface
//  - check that callback can be changed properly and that it is called
struct current_iface_test
{
        current_interface& iface;
        clk_interface&     clk;
        std::string_view   name = "curr_test";

        t::coroutine< void > run( auto& mem, collector& coll )
        {

                em::defer d = retain_callback( iface );

                std::optional< uint32_t > opt_curr = std::nullopt;
                std::span< uint16_t >     prof;
                current_cb                ccb{ [&]( uint32_t curr, std::span< uint16_t > profile ) {
                        opt_curr = curr;
                        prof     = profile;
                } };
                iface.set_current_callback( ccb );
                co_await t::expect( coll, &iface.get_current_callback() == &ccb );

                wait_for( clk, 10_ms );

                co_await store_metric( mem, coll, "prof size", prof.size() );
                co_await t::expect( coll, !prof.empty() );

                co_await t::expect( coll, opt_curr.has_value() );
                co_await store_metric( mem, coll, "curr", opt_curr.value() );
        }
};

inline void setup_interface_tests(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    t::collector&             coll,
    clk_interface&            clk,
    com_interface&            comms,
    period_interface&         period,
    pwm_motor_interface&      pwm,
    vcc_interface&            vcc,
    temperature_interface&    temp,
    position_interface&       pos,
    current_interface&        curr,
    em::result&               res )
{
        setup_utest< clock_test >( mem, reac, coll, res, clk );
        setup_utest< comms_echo_test >( mem, reac, coll, res, clk, comms );
        setup_utest< comms_timeout_test >( mem, reac, coll, res, comms );
        setup_utest< period_iface_test >( mem, reac, coll, res, clk, period );
        setup_utest< pwm_motor_test >( mem, reac, coll, res, period, pwm );
        setup_utest< vcc_test >( mem, reac, coll, res, vcc );
        setup_utest< temperature_test >( mem, reac, coll, res, temp );
        setup_utest< position_test >( mem, reac, coll, res, pos, clk );
        setup_utest< current_iface_test >( mem, reac, coll, res, curr, clk );
}

}  // namespace servio::drv::tests
