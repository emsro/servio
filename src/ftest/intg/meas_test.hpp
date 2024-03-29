#pragma once

#include "cnv/utils.hpp"
#include "core/core.hpp"
#include "drv/callbacks.hpp"
#include "drv/interfaces.hpp"
#include "drv/retainers.hpp"
#include "ftest/is_powerless.hpp"
#include "ftest/utest.hpp"

namespace servio::ftest::tests
{

struct meas_cur_test
{
        t::parameters& params;

        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;
        core::core&           cor;

        std::string_view name = "measure_current";

        t::coroutine< void > run( auto&, collector& coll )
        {
                if ( co_await is_powerless( params ) )
                        co_await t::skip();
                setup_poweroff( cor.ctl );

                em::defer             d = drv::retain_callback( curr );
                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                motor.set_power( std::numeric_limits< int16_t >::max() / 4 );

                t::node_id dnid =
                    co_await coll->set( "data", em::contiguous_container_type::ARRAY );
                t::node_id tnid =
                    co_await coll->set( "time", em::contiguous_container_type::ARRAY );

                base::microseconds start = clk.get_us();

                static constexpr std::size_t iters = 1000;
                float                        sum   = 0;
                for ( std::size_t i = 0; i < iters; i++ ) {
                        float current = cnv::current( cor.conv, curr, motor );
                        sum += current;
                        coll->append( dnid, current );
                        coll->append( tnid, std::chrono::microseconds{ clk.get_us() } );
                }
                co_await t::expect( !em::almost_equal( sum / iters, 0.f, 0.01f ) );
                motor.set_power( 0 );
        }
};

struct meas_pos_test
{
        t::parameters& params;

        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;
        core::core&           cor;

        std::string_view name = "measure_position";

        t::coroutine< void > run( auto&, collector& coll )
        {
                if ( co_await is_powerless( params ) )
                        co_await t::skip();
                setup_poweroff( cor.ctl );
                em::defer d = drv::retain_callback( curr );

                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                motor.set_power( std::numeric_limits< int16_t >::max() / 4 );
                float start = cor.met.get_position();

                drv::wait_for( clk, 400_us );

                float end = cor.met.get_position();
                motor.set_power( 0 );

                float expected_angle_change = 0.f;
                co_await t::expect( std::abs( start - end ) > expected_angle_change );
                coll->set( "pos1", start );
                coll->set( "pos2", end );
        }
};

struct meas_vel_test
{
        t::parameters& params;

        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;
        core::core&           cor;

        std::string_view name = "measure_vel";

        t::coroutine< void > run( auto&, collector& coll )
        {
                if ( co_await is_powerless( params ) )
                        co_await t::skip();
                setup_poweroff( cor.ctl );
                em::defer d = drv::retain_callback( curr );

                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                t::node_id nid = co_await coll->set( "data", em::contiguous_container_type::ARRAY );
                float      sum = 0.f;
                std::size_t measurements = 1024u;

                motor.set_power( std::numeric_limits< int16_t >::max() / 2 );

                for ( std::size_t i = 0; i < measurements; i++ ) {
                        float vel = cor.met.get_velocity();
                        coll->append( nid, vel );
                        sum += vel;
                }

                motor.set_power( 0 );

                float average_velocity = sum / static_cast< float >( measurements );
                co_await t::expect( !em::almost_equal( average_velocity, 0.f, 0.05f ) );
                coll->set( "average velocity", average_velocity );
        }
};

inline void setup_meas_tests(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    t::collector&             coll,
    t::parameters&            params,
    drv::clk_iface&           clk,
    drv::pwm_motor_iface&     motor,
    drv::curr_iface&          curr,
    core::core&               cor,
    em::result&               res )
{
        setup_utest< meas_vel_test >( mem, reac, coll, res, params, clk, curr, motor, cor );
        setup_utest< meas_pos_test >( mem, reac, coll, res, params, clk, curr, motor, cor );
        setup_utest< meas_cur_test >( mem, reac, coll, res, params, clk, curr, motor, cor );
}

}  // namespace servio::ftest::tests
