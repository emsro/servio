#pragma once

#include "../../cnv/utils.hpp"
#include "../../core/core.hpp"
#include "../../drv/callbacks.hpp"
#include "../../drv/interfaces.hpp"
#include "../../drv/retainers.hpp"
#include "../rewind.hpp"
#include "../utest.hpp"

namespace servio::ftest::intg
{

struct current_ctl_test
{
        t::parameters& params;

        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;

        core::core& cor;

        std::string_view name = "current_ctl_test";

        t::coroutine< void > run( auto&, uctx& ctx )
        {
                em::defer d = setup_poweroff( cor.gov_ );

                float expected = 0.2F;
                cor.gov_.activate( "current", {} );
                auto* p = dynamic_cast< gov::curr::_current_gov* >( cor.gov_.active_governor() );
                co_await ctx.expect( p != nullptr );
                p->set_goal_current( expected );

                float                        avg_curr = 0;
                static constexpr std::size_t count    = 10000000;

                drv::wait_for( clk, 350_ms );
                for ( std::size_t i : em::range( count ) ) {
                        std::ignore   = i;
                        float current = cnv::current( cor.conv, curr, motor );
                        avg_curr += current;
                }
                avg_curr /= static_cast< float >( count );

                ctx.coll.set( "measured", avg_curr );
                ctx.coll.set( "expected", expected );
                co_await ctx.expect( em::almost_equal( avg_curr, expected, 0.1F ) );
        }
};

struct sign_test
{
        t::parameters& params;

        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pos_iface&       pos;
        drv::pwm_motor_iface& motor;

        core::core& cor;

        std::string_view name = "sign_test";

        t::coroutine< void > run( auto&, uctx& ctx )
        {
                em::defer d = setup_poweroff( cor.gov_ );
                rewind( cor, clk, pos, 250_ms, { 0.0f, 0.3f }, 0.5f, [] {} );

                t::node_id did =
                    co_await ctx.coll.set( "data", em::contiguous_container_type::ARRAY );

                cor.gov_.activate( "power", {} );
                auto* p = dynamic_cast< gov::pow::_power_gov* >( cor.gov_.active_governor() );
                co_await ctx.expect( p != nullptr );
                p->power = p_low / 2.F;
                drv::wait_for( clk, 200_ms );
                std::size_t count = 50;

                float csum = 0;
                float vsum = 0;

                for ( std::size_t i : em::range( count ) ) {
                        std::ignore = i;
                        t::node_id nid =
                            co_await ctx.coll.append( did, em::contiguous_container_type::OBJECT );

                        float current = servio::cnv::current( cor.conv, curr, motor );
                        float pos     = cor.met.get_position();
                        float vel     = cor.met.get_velocity();

                        csum += current;
                        vsum += vel;

                        ctx.coll.set( nid, "now", clk.get_us() );
                        ctx.coll.set( nid, "cur", current );
                        ctx.coll.set( nid, "pos", pos );
                        ctx.coll.set( nid, "vel", vel );
                }
                co_await ctx.expect( std::signbit( csum ) == std::signbit( vsum ) );
                co_await ctx.expect( cor.gov_.deactivate() == SUCCESS );
        }
};

inline void setup_ctl_test(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    ftest::uctx&              ctx,
    t::parameters&            params,
    drv::clk_iface&           clk,
    drv::pwm_motor_iface&     motor,
    drv::curr_iface&          curr,
    drv::pos_iface&           pos,
    core::core&               cor,
    status&                   res )
{
        setup_utest< current_ctl_test >( mem, reac, ctx, res, params, clk, curr, motor, cor );
        setup_utest< sign_test >( mem, reac, ctx, res, params, clk, curr, pos, motor, cor );
}

}  // namespace servio::ftest::intg
