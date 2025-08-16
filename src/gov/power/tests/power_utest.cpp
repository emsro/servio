
#include "../../tests/gov_fixture.hpp"
#include "../power.hpp"

#include <gtest/gtest.h>

namespace servio::gov::pow::tests
{
TEST_F( gov_fixture, vel )
{

        for ( float pow : { 1.F, 0.F, -1.F } ) {
                _power_gov gov;

                auto do_cmd = [&]( std::string_view cmd ) {
                        parser::parser p{ cmd };
                        char           buff[128];
                        json::jval_ser jser{ buff };
                        auto           s = gov.on_cmd( p, jser );
                        EXPECT_EQ( s, SUCCESS ) << "cmd: " << cmd << "\n";
                };

                do_cmd( std::format( "set {}", pow ) );

                for ( std::size_t i : em::range( 100u ) ) {
                        std::ignore      = i;
                        microseconds now = 5_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.power, pow, 0.02F )
                    << "angle: " << pow << " motor power: " << motor.power;
        }
}
}  // namespace servio::gov::pow::tests
