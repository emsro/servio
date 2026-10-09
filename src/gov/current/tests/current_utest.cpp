
#include "../../../base.hpp"
#include "../../../lib/json_ser.hpp"
#include "../../../lib/parser.hpp"
#include "../../../status.hpp"
#include "../../tests/gov_fixture.hpp"
#include "../current.hpp"

#include <cstddef>
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
}  // namespace servio::gov::curr::tests
