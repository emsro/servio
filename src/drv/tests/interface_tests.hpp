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

// Test that clock of chip works correctly:
//  - time increases
struct clock_test
{
        em::testing::collector& coll;
        clk_interface&          clk;

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

// Test comms:
//  - send a message and wait for reply
//  - assumes that comms is connected in a way that echo goes back
struct comms_echo_test
{
        com_interface&   comms;
        std::string_view name = "comms_echo";

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

// Test comms:
//  - send message and immediatly send another one - triggering timeout
//  - check that timeout was reported - operation errored
//  - assumes that comms is connected in a way that echo goes back
//  - send another message a while after it
struct comms_timeout_test
{
        com_interface&   comms;
        std::string_view name = "comms_timeout";

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

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                auto d = retain_callback( iface );
                co_await rec.expect( iface.stop() == em::SUCCESS );

                std::size_t counter = 0;
                period_cb   pcb{ [&] {
                        counter += 1;
                } };
                iface.set_period_callback( pcb );
                co_await rec.expect( &iface.get_period_callback() == &pcb );

                wait_for( clk, 10_ms );
                rec.expect( counter == 0 );

                co_await rec.expect( iface.start() != em::SUCCESS );
                wait_for( clk, 10_ms );
                rec.expect( counter != 0 );
        }
};

// Test PWM motor interface
//  - just checks whenever direction matches set power
// note: force_stop not tested, which is kinda mistake but it's also hard to test :)
struct pwm_motor_test
{
        pwm_motor_interface& iface;
        std::string_view     name = "pwm_motor";

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

// Test vcc reading
//  - just check that voltage is nonzero
struct vcc_test
{
        vcc_interface&          iface;
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

// Test temperature reading
//  - just check that temp is nonzero
struct temperature_test
{
        temperature_interface&  iface;
        em::testing::collector& coll;
        std::string_view        name = "temp_test";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                auto temp = iface.get_temperature();
                coll.set( 0, "temp", temp );
                rec.expect( temp != 0 );

                co_return;
        }
};

// Test position iface
//  - check that callback can be changed properly
//  - check that callback was called
struct position_test
{
        position_interface&     iface;
        clk_interface&          clk;
        em::testing::collector& coll;
        std::string_view        name = "pos_test";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                em::defer d = retain_callback( iface );

                std::optional< uint32_t > opt_pos = std::nullopt;
                position_cb               pcb{ [&]( uint32_t pos ) {
                        opt_pos = pos;
                } };
                iface.set_position_callback( pcb );
                co_await rec.expect( &iface.get_position_callback() == &pcb );

                wait_for( clk, 10_ms );

                rec.expect( opt_pos.has_value() );
        }
};

inline void setup_interface_tests(
    em::pmr::memory_resource& mem,
    em::testing::reactor&     reac,
    em::testing::collector&   coll,
    clk_interface&            clk,
    com_interface&            comms,
    period_interface&         period,
    pwm_motor_interface&      pwm,
    vcc_interface&            vcc,
    temperature_interface&    temp,
    position_interface&       pos,
    em::result&               res )
{
        setup_utest< clock_test >( mem, reac, res, coll, clk );
        setup_utest< comms_echo_test >( mem, reac, res, comms );
        setup_utest< comms_timeout_test >( mem, reac, res, comms );
        setup_utest< period_iface_test >( mem, reac, res, clk, period );
        setup_utest< pwm_motor_test >( mem, reac, res, pwm );
        setup_utest< vcc_test >( mem, reac, res, vcc, coll );
        setup_utest< temperature_test >( mem, reac, res, temp, coll );
        setup_utest< position_test >( mem, reac, res, pos, clk, coll );
}

}  // namespace servio::drv::tests
