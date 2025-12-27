
#include "../../tests/gov_fixture.hpp"
#include "../velocity.hpp"

#include <gtest/gtest.h>

namespace servio::gov::vel::tests
{
TEST_F( gov_fixture, vel )
{
        GTEST_SKIP() << "uncalibrated";

        for ( float vel : { 1.F, 2.F, 1.F, -1.F } ) {
                _velocity_gov gov;

                auto do_cmd = [&]( std::string_view cmd ) {
                        parser::parser p{ cmd };
                        char           buff[128];
                        json::jval_ser jser{ buff };
                        auto           s = gov.on_cmd( p, jser );
                        EXPECT_EQ( s, SUCCESS ) << "cmd: " << cmd << "\n";
                };

                do_cmd( "cfg set curr_loop_p 0.0625" );
                do_cmd( "cfg set curr_loop_i 5.12" );
                do_cmd( "cfg set curr_loop_d 0.0" );
                do_cmd( "cfg set vel_loop_p 16.0" );
                do_cmd( "cfg set vel_loop_i 500" );
                do_cmd( "cfg set vel_loop_d 0.0" );
                do_cmd( "cfg set static_friction_scale 1" );
                do_cmd( std::format( "set {}", vel ) );

                for ( std::size_t i : em::range( 100u ) ) {
                        std::ignore      = i;
                        microseconds now = 5_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.velocity, vel, 0.02F )
                    << "angle: " << vel << " motor velocity: " << motor.velocity;
        }
}
}  // namespace servio::gov::vel::tests
