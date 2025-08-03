
#include "../../tests/gov_fixture.hpp"
#include "../position.hpp"

#include <gtest/gtest.h>

namespace servio::gov::pos::tests
{
TEST_F( gov_fixture, pos )
{
        for ( float angle : { 1.F, 2.F, 1.F, -1.F } ) {
                _position_gov gov;

                auto do_cmd = [&]( std::string_view cmd ) {
                        parser::parser p{ parser::lexer{ cmd } };
                        char           buff[128];
                        json::jval_ser jser{ buff };
                        auto           s = gov.on_cmd( p, jser );
                        EXPECT_EQ( s, SUCCESS ) << "cmd: " << cmd << "\n";
                };

                do_cmd( "cfg set curr_loop_p 0.0625" );
                do_cmd( "cfg set curr_loop_i 0.000'005'12" );
                do_cmd( "cfg set curr_loop_d 0.03125" );
                do_cmd( "cfg set pos_loop_p 0.03" );
                do_cmd( "cfg set pos_loop_i 0.0" );
                do_cmd( "cfg set pos_loop_d 0.05" );
                do_cmd( "cfg set static_friction_scale 1" );
                do_cmd( std::format( "set {}", angle ) );

                for ( std::size_t i : em::range( 100u ) ) {
                        std::ignore      = i;
                        microseconds now = 5_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.position(), angle, 0.02F )
                    << "angle: " << angle << " motor position: " << motor.position();
        }
}
}  // namespace servio::gov::pos::tests
