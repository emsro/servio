
#include "../../tests/gov_fixture.hpp"
#include "../current.hpp"

#include <gtest/gtest.h>

namespace servio::gov::curr::tests
{
TEST_F( gov_fixture, curr )
{
        for ( float curr : { 0.1F, 0.2F, -0.1F, 0.F, 0.1F } ) {
                _current_gov gov;

                auto do_cmd = [&]( std::string_view cmd ) {
                        parser::parser p{ parser::lexer{ cmd } };
                        char           buff[128];
                        json::jval_ser jser{ buff };
                        auto           s = gov.on_cmd( p, jser );
                        EXPECT_EQ( s, SUCCESS ) << "cmd: " << cmd << "\n";
                };

                do_cmd( "cfg set loop_p 0.0625" );
                do_cmd( "cfg set loop_i 0.000'005'12" );
                do_cmd( "cfg set loop_d 0.03125" );
                do_cmd( std::format( "set {}", curr ) );

                for ( std::size_t i : em::range( 100u ) ) {
                        std::ignore      = i;
                        microseconds now = 5_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.current, curr, 0.02F )
                    << "curr: " << curr << " motor current: " << motor.current;
        }
}
}  // namespace servio::gov::curr::tests
