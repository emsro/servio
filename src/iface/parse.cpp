
#include "../lib/parser.hpp"
#include "./def.hpp"
#include "./kvals_parse.hpp"

namespace servio::iface
{
namespace
{

using expr = vari::typelist< std::string_view, float, int32_t >;

opt< mode_stmt > _mode_stmt( parser::parser& p )
{
        if ( auto r = kvals_parse< mode_vals >::parse( p ) )
                return mode_stmt{ .arg = std::move( r ).vval() };
        return {};
}

vari::vopt< cfg_vals > _cfg_vals( parser::parser& p )
{
        return kvals_parse< cfg_vals >::parse( p );
}

vari::vval< stmt, invalid_stmt > _parse( parser::parser& p )
{
        // XXX, missing tests:
        // - no overflow after parsed message
        auto id = p.exp( "id"_a );
        if ( !id ) {
                return invalid_stmt{};
        } else if ( *id == "mode" ) {
                if ( auto r = _mode_stmt( p ) )
                        return *r;
                return invalid_stmt{};
        } else if ( *id == "prop" ) {
                auto pr = p.exp_any< prop_key >();
                if ( !pr )
                        return invalid_stmt{};
                return prop_stmt{
                    .prop = *pr,
                };
        } else if ( *id == "cfg" ) {
                auto sub = p.exp( "id"_a );
                if ( !sub )
                        return invalid_stmt{};
                if ( *sub == "set" ) {
                        if ( auto r = _cfg_vals( p ) )
                                return cfg_set_stmt{ .val = std::move( r ).vval() };
                } else if ( *sub == "get" ) {
                        auto m = p.exp_any< cfg_key >();
                        if ( !m )
                                return invalid_stmt{};
                        return cfg_get_stmt{
                            .k = *m,
                        };
                } else if ( *sub == "commit" ) {
                        return cfg_commit_stmt{};
                } else if ( *sub == "clear" ) {
                        return cfg_clear_stmt{};
                }
        } else if ( *id == "info" ) {
                return info_stmt{};
        }
        return invalid_stmt{};
}

}  // namespace

vari::vval< stmt, invalid_stmt > parse( std::string_view inpt )
{
        parser::lexer  l{ inpt };
        parser::parser p{ std::move( l ) };

        using R = vari::vval< stmt, invalid_stmt >;
        return _parse( p ).visit(
            [&]( invalid_stmt& is ) -> R {
                    return is;
            },
            [&]( vari::vval< stmt > st ) -> R {
                    if ( !p.l )
                            return st;
                    return p.l->next().visit(
                        [&]( parser::end& ) -> R {
                                return st;
                        },
                        [&]( vari::vref< parser::tok > ) -> R {
                                return invalid_stmt{};
                        } );
            } );
}

}  // namespace servio::iface
