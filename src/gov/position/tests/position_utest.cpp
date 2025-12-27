
#include "../../tests/gov_fixture.hpp"
#include "../position.hpp"

#include <gtest/gtest.h>

namespace servio::gov::pos::tests
{
TEST_F( gov_fixture, pos )
{
        GTEST_SKIP() << "uncalibrated";
        for ( float angle : { 1.F, 2.F, 1.F, -1.F } ) {
                _position_gov gov;

                auto do_cmd = [&]( std::string_view cmd ) {
                        parser::parser p{ cmd };
                        char           buff[128];
                        json::jval_ser jser{ buff };
                        auto           s = gov.on_cmd( p, jser );
                        EXPECT_EQ( s, SUCCESS ) << "cmd: " << cmd << "\n";
                };

                auto set_cfg = [&]( std::string_view key, auto value ) {
                        auto s = gov.get_cfg()->on_cmd_set( key, value );
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

                for ( std::size_t i : em::range( 500u ) ) {
                        std::ignore      = i;
                        microseconds now = 1_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.position(), angle, 0.02F )
                    << "angle: " << angle << " motor position: " << motor.position();
        }
}
}  // namespace servio::gov::pos::tests
