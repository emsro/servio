#include "../def.hpp"

#include <gtest/gtest.h>
#include <string_view>
#include <vari/vopt.h>
#include <vari/vval.h>

namespace servio::iface
{
namespace
{
template < typename S >
void test_valid_parse( std::string_view inpt, S expected_stmt )
{
        auto res = parse( inpt );
        res.visit(
            [&]( vari::vref< stmt > s ) {
                    vari::vval< stmts > expected{ expected_stmt };
                    EXPECT_EQ( s->sub, expected );
            },
            [&]( invalid_stmt& ) {
                    FAIL() << "expected valid parse";
            } );
}

void test_invalid_parse( std::string_view inpt, parse_status st )
{
        auto res = parse( inpt );
        res.visit(
            [&]( vari::vref< stmt > ) {
                    FAIL() << "expected invalid parse";
            },
            [&]( invalid_stmt& s ) {
                    EXPECT_EQ( s.st, st ) << "inpt: " << inpt << "\n"
                                          << "st: " << to_str( s.st ) << "\n";
            } );
}
}  // namespace

TEST( IfaceParse, ValidParse )
{
        test_valid_parse( "mode disengaged", mode_stmt{ mode_disengaged_stmt{} } );
        test_valid_parse( "mode power 42", mode_stmt{ mode_power_stmt{ 42.0F } } );
        test_valid_parse( "mode current -1", mode_stmt{ mode_current_stmt{ -1.0F } } );
        test_valid_parse( "mode velocity 0.0", mode_stmt{ mode_velocity_stmt{ 0.0F } } );
        test_valid_parse( "mode position 0", mode_stmt{ mode_position_stmt{ 0.0F } } );
        test_valid_parse( "mode position 3.14", mode_stmt{ mode_position_stmt{ 3.14F } } );

        test_valid_parse( "prop mode", prop_stmt{ property::mode } );
        test_valid_parse( "prop current", prop_stmt{ property::current } );
        test_valid_parse( "prop vcc", prop_stmt{ property::vcc } );
        test_valid_parse( "prop temp", prop_stmt{ property::temp } );
        test_valid_parse( "prop position", prop_stmt{ property::position } );
        test_valid_parse( "prop velocity", prop_stmt{ property::velocity } );

        test_valid_parse(
            "cfg set foo bar",
            cfg_stmt{ cfg_set_stmt{ .field = "foo", .value.data = iface::string{ "bar" } } } );
        test_valid_parse( "cfg get foo", cfg_stmt{ cfg_get_stmt{ "foo" } } );
        test_valid_parse(
            "cfg list5 level 1 5",
            cfg_stmt{ cfg_list5_stmt{ .level = "level", .offset = 1, .n = 5 } } );
        test_valid_parse( "cfg commit", cfg_stmt{ cfg_commit_stmt{} } );
        test_valid_parse( "cfg clear", cfg_stmt{ cfg_clear_stmt{} } );
}

TEST( IfaceParse, InvalidParse )
{
        test_invalid_parse( "", parse_status::CMD_MISSING );
        test_invalid_parse( "mode", parse_status::CMD_MISSING );
        test_invalid_parse( "prop", parse_status::ARG_MISSING );
        test_invalid_parse( "cfg", parse_status::CMD_MISSING );
        test_invalid_parse( "cfg s", parse_status::UNKNOWN_CMD );
        test_invalid_parse( "cfg commit boo", parse_status::UNEXPECTED_ARG );
}

}  // namespace servio::iface
