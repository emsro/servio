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
void test_valid_parse( parser::parser& p, std::string_view inpt, S expected_stmt )
{
        p        = parser::parser{ inpt };
        auto res = parse( p );
        res.visit(
            [&]( vari::vref< stmt > s ) {
                    vari::vval< stmts > expected{ std::move( expected_stmt ) };
                    EXPECT_EQ( s->sub, expected );
            },
            [&]( invalid_stmt& ) {
                    FAIL() << "expected valid parse";
            } );
}

void test_invalid_parse( parser::parser& p, std::string_view inpt, parse_status st )
{
        auto res = parse( p );
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
        parser::parser p{ "" };
        test_valid_parse( p, "gov activate power ", gov_stmt{ gov_activate_stmt{ "power" } } );
        test_valid_parse( p, "gov activate current ", gov_stmt{ gov_activate_stmt{ "current" } } );
        test_valid_parse(
            p, "gov activate velocity ", gov_stmt{ gov_activate_stmt{ "velocity" } } );
        test_valid_parse(
            p, "gov activate position ", gov_stmt{ gov_activate_stmt{ "position" } } );
        test_valid_parse( p, "gov deactivate ", gov_stmt{ gov_deactivate_stmt{} } );
        test_valid_parse( p, "gov list 0 ", gov_stmt{ gov_list_stmt{ 0 } } );
        test_valid_parse( p, "gov wololo ", gov_stmt{ gov_forward{ "wololo", p } } );
        test_valid_parse( p, "gov wololo magic ", gov_stmt{ gov_forward{ "wololo", p } } );

        test_valid_parse( p, "prop current", prop_stmt{ property::current } );
        test_valid_parse( p, "prop vcc", prop_stmt{ property::vcc } );
        test_valid_parse( p, "prop temp", prop_stmt{ property::temp } );
        test_valid_parse( p, "prop position", prop_stmt{ property::position } );
        test_valid_parse( p, "prop velocity", prop_stmt{ property::velocity } );

        test_valid_parse(
            p,
            "cfg set foo bar",
            cfg_stmt{ cfg_set_stmt{ .field = "foo", .value = { iface::string{ "bar" } } } } );
        test_valid_parse( p, "cfg get foo", cfg_stmt{ cfg_get_stmt{ "foo" } } );
        test_valid_parse( p, "cfg list 1", cfg_stmt{ cfg_list_stmt{ .index = 1 } } );
        test_valid_parse( p, "cfg commit", cfg_stmt{ cfg_commit_stmt{} } );
        test_valid_parse( p, "cfg clear", cfg_stmt{ cfg_clear_stmt{} } );
}

TEST( IfaceParse, InvalidParse )
{
        parser::parser p{ "" };
        test_invalid_parse( p, "", parse_status::CMD_MISSING );
        test_invalid_parse( p, "mode", parse_status::CMD_MISSING );
        test_invalid_parse( p, "prop", parse_status::ARG_MISSING );
        test_invalid_parse( p, "cfg", parse_status::CMD_MISSING );
        test_invalid_parse( p, "cfg s", parse_status::UNKNOWN_CMD );
        test_invalid_parse( p, "cfg commit boo", parse_status::UNEXPECTED_ARG );
}

}  // namespace servio::iface
