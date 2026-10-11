
#include "../../../base.hpp"
#include "../../../lib/json_ser.hpp"
#include "../../../lib/parser.hpp"
#include "../../../status.hpp"
#include "../../governor_manager.hpp"
#include "../../tests/gov_fixture.hpp"
#include "../position.hpp"

#include <cmath>
#include <cstddef>
#include <emlabcpp/pmr/new_delete_resource.hpp>
#include <emlabcpp/range.hpp>
#include <format>
#include <gtest/gtest.h>
#include <string_view>
#include <tuple>

namespace servio::gov::pos::tests
{
TEST_F( gov_fixture, pos )
{
        GTEST_SKIP() << "uncalibrated";
        for ( float angle : { 1.F, 2.F, 1.F, -1.F } ) {
                _position_gov gov;

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

                set_cfg( "curr_loop_p", 0.0625F );
                set_cfg( "curr_loop_i", 0.0000005F );
                set_cfg( "curr_loop_d", 0.0F );
                set_cfg( "pos_loop_p", 2.0F );
                set_cfg( "pos_loop_i", 0.0F );
                set_cfg( "pos_loop_d", 0.0F );
                set_cfg( "static_friction_scale", 1.0F );
                set_cfg( "pos_to_curr_lim_scale", 10.0F );
                do_cmd( std::format( "set {}", angle ) );

                for ( std::size_t const i : em::range( 500u ) ) {
                        std::ignore            = i;
                        microseconds const now = 1_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.position(), angle, 0.02F )
                    << "angle: " << angle << " motor position: " << motor.position();
        }
}

TEST( pos_gov, power_and_outer_integrator_stay_in_limits )
{
        for ( float const goal : { 100.F, -100.F } ) {
                _position_gov gov;
                gov.cfg.curr_loop_p = 100.F;
                gov.cfg.pos_loop_i  = 1000.F;
                gov.get_cfg()->apply_all();
                gov.goal_pos = goal;

                for ( std::size_t const i : em::range( 100U ) ) {
                        microseconds const now = 1_ms * ( i + 1 );
                        gov.metrics_irq( now, 0.F, 0.F, true );
                        pwr const p = gov.current_irq( now, 0.F );
                        EXPECT_LE( std::abs( *p ), 1.F ) << "goal: " << goal;
                        EXPECT_LE( gov.pos_pid.i_sum, gov.cfg.curr_lim_max ) << "goal: " << goal;
                        EXPECT_GE( gov.pos_pid.i_sum, gov.cfg.curr_lim_min ) << "goal: " << goal;
                }
        }
}

TEST( pos_gov, engage_holds_measured_position )
{
        _position_gov gov;
        gov.get_cfg()->apply_all();

        float const      theta = 1.F;
        governor_manager gm;
        ASSERT_TRUE( gm.add_gov( gov ) );
        ASSERT_EQ(
            gm.activate( "position", em::pmr::new_delete_resource(), 10_s, theta, 0.F ),
            status::success );

        for ( std::size_t const i : em::range( 100U ) ) {
                microseconds const now = 10_s + 1_ms * i;
                gm.metrics_irq( now, theta, 0.F, false );
                pwr const p = gm.current_irq( now, 0.F );
                EXPECT_NEAR( *p, 0.F, 1e-3F );
        }
        EXPECT_EQ( gov.goal_pos, theta );
}
}  // namespace servio::gov::pos::tests
