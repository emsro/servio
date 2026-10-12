
#include "../../../base.hpp"
#include "../../../lib/json_ser.hpp"
#include "../../../lib/parser.hpp"
#include "../../../status.hpp"
#include "../../governor_manager.hpp"
#include "../../tests/gov_fixture.hpp"
#include "../current.hpp"

#include <cstddef>
#include <emlabcpp/pmr/new_delete_resource.hpp>
#include <emlabcpp/range.hpp>
#include <format>
#include <gtest/gtest.h>
#include <string_view>
#include <tuple>

namespace servio::gov::curr::tests
{
TEST_F( gov_fixture, curr )
{
        for ( float curr : { 0.1F, 0.2F, -0.1F, 0.F, 0.1F } ) {
                _current_gov gov;

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
                set_cfg( "curr_loop_i", 5.12F );
                set_cfg( "curr_loop_d", 0.0F );
                set_cfg( "pos_lim_min", -1e30F );
                set_cfg( "pos_lim_max", 1e30F );
                do_cmd( std::format( "set {}", curr ) );

                for ( std::size_t const i : em::range( 100u ) ) {
                        std::ignore            = i;
                        microseconds const now = 5_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.current, curr, 0.02F )
                    << "curr: " << curr << " motor current: " << motor.current;
        }
}

TEST( curr_gov, engage_after_idle_has_no_output_step )
{
        _current_gov gov;
        gov.cfg.curr_loop_p = 0.F;
        gov.cfg.curr_loop_i = 3.F;
        gov.get_cfg()->apply_all();

        governor_manager gm;
        ASSERT_TRUE( gm.add_gov( gov ) );
        auto const tick = [&]( microseconds now ) {
                gm.metrics_irq( now, 0.F, 0.F, false );
                return gm.current_irq( now, 0.F );
        };

        ASSERT_EQ(
            gm.activate( "current", em::pmr::new_delete_resource(), 0_ms, 0.F, 0.F ),
            status::success );
        gov.set_goal_current( 0.5F );
        for ( std::size_t const i : em::range( 100U ) )
                std::ignore = tick( 1_ms * ( i + 1 ) );
        ASSERT_EQ( gm.deactivate(), status::success );

        ASSERT_EQ(
            gm.activate( "current", em::pmr::new_delete_resource(), 10_s, 0.F, 0.F ),
            status::success );
        EXPECT_EQ( gov.goal, 0.F );
        gov.set_goal_current( 0.1F );
        std::ignore = tick( 10_s );
        pwr const p = tick( 10_s + 100_us );
        EXPECT_NEAR( *p, 0.F, 1e-3F );
}
}  // namespace servio::gov::curr::tests
