#pragma once

#include "../../cnv/utils.hpp"
#include "../../core/core.hpp"
#include "../../drv/callbacks.hpp"
#include "../../drv/interfaces.hpp"
#include "../../drv/retainers.hpp"
#include "../utest.hpp"

namespace servio::ftest::intg
{

struct meas_cur_test
{
        t::parameters& params;

        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;
        core::core&           cor;

        std::string_view name = "measure_current";

        t::coroutine< void > run( auto&, uctx& ctx )
        {
                setup_poweroff( cor.ctl );

                em::defer             d = drv::retain_callback( curr );
                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                motor.set_power( p_max );

                t::node_id dnid =
                    co_await ctx.coll.set( "data", em::contiguous_container_type::ARRAY );
                t::node_id tnid =
                    co_await ctx.coll.set( "time", em::contiguous_container_type::ARRAY );

                static constexpr std::size_t iters = 50;
                float                        sum   = 0;
                for ( std::size_t i = 0; i < iters; i++ ) {
                        float current = cnv::current( cor.conv, curr, motor );
                        sum += current;
                        ctx.coll.append( dnid, current );
                        ctx.coll.append( tnid, std::chrono::microseconds{ clk.get_us() } );
                        drv::wait_for( clk, 10_ms );
                }
                co_await t::expect( !em::almost_equal( sum / iters, 0.f, 0.01f ) );
                motor.set_power( p_max );
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

        t::coroutine< void > run( auto&, uctx& ctx )
        {
                setup_poweroff( cor.ctl );
                em::defer d = drv::retain_callback( curr );

                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                motor.set_power( p_max / 4.F );
                float start = cor.met.get_position();

                drv::wait_for( clk, 400_us );

                float end = cor.met.get_position();
                motor.set_power( 0_pwr );

                float expected_angle_change = 0.F;
                co_await ctx.expect( std::abs( start - end ) > expected_angle_change );
                ctx.coll.set( "pos1", start );
                ctx.coll.set( "pos2", end );
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

        t::coroutine< void > run( auto&, uctx& ctx )
        {
                setup_poweroff( cor.ctl );
                em::defer d = drv::retain_callback( curr );

                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                t::node_id nid =
                    co_await ctx.coll.set( "data", em::contiguous_container_type::ARRAY );
                float       sum          = 0.f;
                std::size_t measurements = 1024u;

                motor.set_power( p_max / 2.F );

                for ( std::size_t i = 0; i < measurements; i++ ) {
                        float vel = cor.met.get_velocity();
                        ctx.coll.append( nid, vel );
                        sum += vel;
                }

                motor.set_power( 0_pwr );

                float average_velocity = sum / static_cast< float >( measurements );
                co_await ctx.expect( !em::almost_equal( average_velocity, 0.f, 0.01f ) );
                ctx.coll.set( "average velocity", average_velocity );
        }
};

inline void setup_meas_tests(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    uctx&                     ctx,
    t::parameters&            params,
    drv::clk_iface&           clk,
    drv::pwm_motor_iface&     motor,
    drv::curr_iface&          curr,
    core::core&               cor,
    status&                   res )
{
        setup_utest< meas_vel_test >( mem, reac, ctx, res, params, clk, curr, motor, cor );
        setup_utest< meas_pos_test >( mem, reac, ctx, res, params, clk, curr, motor, cor );
        setup_utest< meas_cur_test >( mem, reac, ctx, res, params, clk, curr, motor, cor );
}

}  // namespace servio::ftest::intg
