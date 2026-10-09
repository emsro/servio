
#include "../../../base.hpp"
#include "../../../lib/json_ser.hpp"
#include "../../../lib/parser.hpp"
#include "../../../status.hpp"
#include "../../tests/gov_fixture.hpp"
#include "../power.hpp"

#include <cstddef>
#include <emlabcpp/range.hpp>
#include <format>
#include <gtest/gtest.h>
#include <string_view>
#include <tuple>

namespace servio::gov::pow::tests
{
TEST_F( gov_fixture, vel )
{

        for ( float pow : { 1.F, 0.F, -1.F } ) {
                _power_gov gov;

                auto const do_cmd = [&]( std::string_view cmd ) {
                        parser::parser p{ cmd };
                        char           buff[128];
                        json::jval_ser jser{ buff };
                        auto const     s = gov.on_cmd( p, jser );
                        EXPECT_EQ( s, status::success ) << "cmd: " << cmd << "\n";
                };

                do_cmd( std::format( "set {}", pow ) );

                for ( std::size_t const i : em::range( 100u ) ) {
                        std::ignore            = i;
                        microseconds const now = 5_ms * i;
                        this->tick( gov, now );
                }

                EXPECT_NEAR( motor.power, pow, 0.02F )
                    << "angle: " << pow << " motor power: " << motor.power;
        }
}
}  // namespace servio::gov::pow::tests
