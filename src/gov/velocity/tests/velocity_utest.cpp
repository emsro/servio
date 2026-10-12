
#include "../../../base.hpp"
#include "../../../lib/json_ser.hpp"
#include "../../../lib/parser.hpp"
#include "../../../status.hpp"
#include "../../governor_manager.hpp"
#include "../../tests/gov_fixture.hpp"
#include "../velocity.hpp"

#include <cmath>
#include <cstddef>
#include <emlabcpp/pmr/new_delete_resource.hpp>
#include <emlabcpp/range.hpp>
#include <format>
#include <gtest/gtest.h>
#include <string_view>
#include <tuple>

namespace servio::gov::vel::tests
{
TEST_F( gov_fixture, vel )
{
        for ( float vel : { 1.F, 2.F, 1.F, -1.F } ) {
                _velocity_gov gov;

                auto const do_cmd = [&]( std::string_view cmd ) {
                        parser::parser p{ cmd };
                        char           buff[128];
                        json::jval_ser jser{ buff };
                        auto const     s = gov.on_cmd( p, jser );
                        EXPECT_EQ( s, status::success ) << "cmd: " << cmd << "\n";
                };

                auto const set_cfg = [&]( std::string_view key, auto value ) {
                        auto const s = gov.get_cfg()->on_cmd_set( key, value );
                        EXPECT_TRUE( !s );
                };

                gov.get_cfg()->apply_all();
                set_cfg( "curr_loop_p", 0.0625F );
                set_cfg( "curr_loop_i", 5.12F );
                set_cfg( "curr_loop_d", 0.0F );
                set_cfg( "vel_loop_p", 16.0F );
                set_cfg( "vel_loop_i", 500.0F );
                set_cfg( "vel_loop_d", 0.0F );
                set_cfg( "static_friction_scale", 1.0F );
                set_cfg( "vel_lim_min", -5.0F );
                set_cfg( "vel_lim_max", 5.0F );
                do_cmd( std::format( "set {}", vel ) );

                for ( std::size_t const i : em::range( 400u ) ) {
                        std::ignore            = i;
                        microseconds const now = 5_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.velocity, vel, 0.02F )
                    << "angle: " << vel << " motor velocity: " << motor.velocity;
        }
}

TEST( vel_gov, power_and_outer_integrator_stay_in_limits )
{
        for ( float const goal : { 100.F, -100.F } ) {
                _velocity_gov gov;
                gov.cfg.curr_loop_p = 100.F;
                gov.cfg.vel_loop_i  = 1000.F;
                gov.get_cfg()->apply_all();
                gov.goal_vel = goal;

                for ( std::size_t const i : em::range( 100U ) ) {
                        microseconds const now = 1_ms * ( i + 1 );
                        gov.metrics_irq( now, 0.F, 0.F, true );
                        pwr const p = gov.current_irq( now, 0.F );
                        EXPECT_LE( std::abs( *p ), 1.F ) << "goal: " << goal;
                        EXPECT_LE( gov.vel_pid.i_sum, gov.cfg.curr_lim_max ) << "goal: " << goal;
                        EXPECT_GE( gov.vel_pid.i_sum, gov.cfg.curr_lim_min ) << "goal: " << goal;
                }
        }
}

TEST( vel_gov, engage_after_idle_has_no_output_step )
{
        _velocity_gov gov;
        gov.cfg.curr_loop_p = 0.F;
        gov.cfg.curr_loop_i = 3.F;
        gov.cfg.vel_loop_p  = 0.F;
        gov.cfg.vel_loop_i  = 10.F;
        gov.get_cfg()->apply_all();

        governor_manager gm;
        ASSERT_TRUE( gm.add_gov( gov ) );
        auto const tick = [&]( microseconds now ) {
                gm.metrics_irq( now, 0.F, 0.F, true );
                return gm.current_irq( now, 0.F );
        };

        ASSERT_EQ(
            gm.activate( "velocity", em::pmr::new_delete_resource(), 0_ms, 0.F, 0.F ),
            status::success );
        gov.goal_vel = 1.F;
        for ( std::size_t const i : em::range( 100U ) )
                std::ignore = tick( 1_ms * ( i + 1 ) );
        ASSERT_EQ( gm.deactivate(), status::success );

        ASSERT_EQ(
            gm.activate( "velocity", em::pmr::new_delete_resource(), 10_s, 0.F, 0.F ),
            status::success );
        gov.goal_vel = 0.1F;
        std::ignore  = tick( 10_s );
        pwr const p  = tick( 10_s + 1_ms );
        EXPECT_NEAR( *p, 0.F, 1e-3F );
}
}  // namespace servio::gov::vel::tests
