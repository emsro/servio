
#include "../lib/parser.hpp"
#include "../status.hpp"
#include "./def.hpp"

namespace servio::iface
{
template < typename T >
using opt = std::optional< T >;

using value_type = vari::typelist< string, float, int32_t >;

enum class arg_status : uint8_t
{
        DEFAULT,
        MISSING,
        FOUND
};

struct arg_def
{
        arg_status                         st;
        char const*                        kw;
        vari::vref< value_type, expr_tok > val;
};

namespace
{
template < typename T >
struct _convert_type_impl;

template < typename T >
using _convert_type = _convert_type_impl< T >;
}  // namespace

template < std::size_t N >
parse_status _arg_parse_impl( parser::parser& p, std::span< arg_def, N > args )
{
        auto load_val = [&]( std::size_t i, vari::vref< parser::expr_t > val ) -> parse_status {
                return args[i].val.visit(
                    [&]( vari::vref< string, float, int32_t, bool > e ) -> parse_status {
                            return parser::load_value( e, val ) ? parse_status::SUCCESS :
                                                                  parse_status::ARG_TYPE_MISMATCH;
                    },
                    [&]( expr_tok& e ) -> parse_status {
                            e.data = val.visit( [&]( auto& x ) {
                                    return vari::vval< parser::expr_t >{ x };
                            } );
                            return parse_status::SUCCESS;
                    } );
        };

        opt< std::size_t > arg_i = 0;

        auto parse_arg = [&]( vari::vref< parser::expr_t > val ) -> parse_status {
                if ( !arg_i )
                        return parse_status::ARG_AFTER_KVAL;
                auto i = *arg_i;
                if ( i >= args.size() )
                        return parse_status::UNEXPECTED_ARG;
                *arg_i += 1;
                args[i].st = arg_status::FOUND;
                return load_val( i, val );
        };

        while ( !p.ended() ) {
                using R = parse_status;
                auto st = p.next().visit(
                    [&]( vari::empty_t ) -> R {
                            return parse_status::SYNTAX_ERROR;
                    },
                    [&]( parser::op_t& ) -> R {
                            return parse_status::SYNTAX_ERROR;
                    },
                    [&]( parser::id_t& id ) -> R {
                            auto pp = p;
                            auto op = pp.op();
                            if ( !op ) {
                                    string st{ id.s };
                                    return parse_arg( st );
                            }
                            p = pp;
                            arg_i.reset();
                            if ( *op != parser::op_t::ASSIGN )
                                    return parse_status::SYNTAX_ERROR;
                            auto e = p.expr();
                            if ( !e )
                                    return parse_status::SYNTAX_ERROR;

                            std::size_t j = 0;
                            for ( ; j < args.size(); j++ )
                                    if ( id.s.compare( args[j].kw ) == 0 )
                                            break;
                            if ( j == args.size() )
                                    return parse_status::UNKNOWN_ARG_KEY;
                            if ( args[j].st == arg_status::FOUND )
                                    return parse_status::ARG_DUPLICATE;
                            args[j].st = arg_status::FOUND;

                            return load_val( j, e.vref() );
                    },
                    [&]( vari::vref< parser::expr_t > val ) -> R {
                            return parse_arg( val );
                    } );
                if ( st != parse_status::SUCCESS )
                        return st;
        }
        for ( std::size_t i = 0; i < args.size(); i++ )
                if ( args[i].st == arg_status::MISSING )
                        return parse_status::ARG_MISSING;
        return parse_status::SUCCESS;
}

inline parse_status _arg_parse( parser::parser& p, std::array< arg_def, 0 > args )
{
        return _arg_parse_impl( p, std::span< arg_def, 0 >( args.data(), 0 ) );
}

inline parse_status _arg_parse( parser::parser& p, std::array< arg_def, 1 > args )
{
        return _arg_parse_impl( p, std::span< arg_def, 1 >( args.data(), 1 ) );
}

template < std::size_t N >
requires( N > 1 )
inline parse_status _arg_parse( parser::parser& p, std::array< arg_def, N > args )
{
        return _arg_parse_impl( p, std::span< arg_def >( args ) );
}

namespace
{

// GEN BEGIN HERE

template <>
struct _convert_type_impl< property >
{
        using type = string;
};

constexpr parse_status _convert( string const& src, property& dst )
{

        if ( src == "mode" ) {
                dst = property::mode;
                return parse_status::SUCCESS;
        }
        if ( src == "current" ) {
                dst = property::current;
                return parse_status::SUCCESS;
        }
        if ( src == "vcc" ) {
                dst = property::vcc;
                return parse_status::SUCCESS;
        }
        if ( src == "temp" ) {
                dst = property::temp;
                return parse_status::SUCCESS;
        }
        if ( src == "position" ) {
                dst = property::position;
                return parse_status::SUCCESS;
        }
        if ( src == "velocity" ) {
                dst = property::velocity;
                return parse_status::SUCCESS;
        }
        return parse_status::UNKNOWN_VALUE;
}

static std::tuple< mode_disengaged_stmt, parse_status > _mode_disengaged( parser::parser& p )
{

        mode_disengaged_stmt     res;
        std::array< arg_def, 0 > arg_defs = {};
        parse_status             st       = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< mode_power_stmt, parse_status > _mode_power( parser::parser& p )
{

        mode_power_stmt          res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "power", .val = res.power } };
        parse_status st = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< mode_current_stmt, parse_status > _mode_current( parser::parser& p )
{

        mode_current_stmt        res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "current", .val = res.current } };
        parse_status st = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< mode_velocity_stmt, parse_status > _mode_velocity( parser::parser& p )
{

        mode_velocity_stmt       res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "velocity", .val = res.velocity } };
        parse_status st = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< mode_position_stmt, parse_status > _mode_position( parser::parser& p )
{

        mode_position_stmt       res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "position", .val = res.position } };
        parse_status st = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< mode_stmt, parse_status > _mode( parser::parser& p )
{
        mode_stmt res;
        auto      id = p.id();
        if ( !id ) {
                return { res, parse_status::CMD_MISSING };
        } else if ( *id == "disengaged" ) {
                auto [substmt, st] = _mode_disengaged( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "power" ) {
                auto [substmt, st] = _mode_power( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "current" ) {
                auto [substmt, st] = _mode_current( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "velocity" ) {
                auto [substmt, st] = _mode_velocity( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "position" ) {
                auto [substmt, st] = _mode_position( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else {
                return { res, parse_status::UNKNOWN_CMD };
        }
        return { res, parse_status::SUCCESS };
}

static std::tuple< prop_stmt, parse_status > _prop( parser::parser& p )
{
        _convert_type< property >::type name;
        prop_stmt                       res;
        std::array< arg_def, 1 >        arg_defs = {
            arg_def{ .st = arg_status::MISSING, .kw = "name", .val = name } };
        parse_status st = _arg_parse( p, arg_defs );

        if ( st == parse_status::SUCCESS )
                st = _convert( name, res.name );

        return { res, st };
}

static std::tuple< cfg_set_stmt, parse_status > _cfg_set( parser::parser& p )
{

        cfg_set_stmt             res;
        std::array< arg_def, 2 > arg_defs = {
            arg_def{ .st = arg_status::MISSING, .kw = "field", .val = res.field },
            arg_def{ .st = arg_status::MISSING, .kw = "value", .val = res.value } };
        parse_status st = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< cfg_get_stmt, parse_status > _cfg_get( parser::parser& p )
{

        cfg_get_stmt             res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::MISSING, .kw = "field", .val = res.field } };
        parse_status st = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< cfg_list5_stmt, parse_status > _cfg_list5( parser::parser& p )
{

        cfg_list5_stmt           res;
        std::array< arg_def, 3 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "level", .val = res.level },
            arg_def{ .st = arg_status::DEFAULT, .kw = "offset", .val = res.offset },
            arg_def{ .st = arg_status::DEFAULT, .kw = "n", .val = res.n } };
        parse_status st = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< cfg_commit_stmt, parse_status > _cfg_commit( parser::parser& p )
{

        cfg_commit_stmt          res;
        std::array< arg_def, 0 > arg_defs = {};
        parse_status             st       = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< cfg_clear_stmt, parse_status > _cfg_clear( parser::parser& p )
{

        cfg_clear_stmt           res;
        std::array< arg_def, 0 > arg_defs = {};
        parse_status             st       = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< cfg_stmt, parse_status > _cfg( parser::parser& p )
{
        cfg_stmt res;
        auto     id = p.id();
        if ( !id ) {
                return { res, parse_status::CMD_MISSING };
        } else if ( *id == "set" ) {
                auto [substmt, st] = _cfg_set( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "get" ) {
                auto [substmt, st] = _cfg_get( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "list5" ) {
                auto [substmt, st] = _cfg_list5( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "commit" ) {
                auto [substmt, st] = _cfg_commit( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "clear" ) {
                auto [substmt, st] = _cfg_clear( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else {
                return { res, parse_status::UNKNOWN_CMD };
        }
        return { res, parse_status::SUCCESS };
}

static std::tuple< info_stmt, parse_status > _info( parser::parser& p )
{

        info_stmt                res;
        std::array< arg_def, 0 > arg_defs = {};
        parse_status             st       = _arg_parse( p, arg_defs );

        return { res, st };
}

static std::tuple< stmt, parse_status > _root( parser::parser& p )
{
        stmt res;
        auto id = p.id();
        if ( !id ) {
                return { res, parse_status::CMD_MISSING };
        } else if ( *id == "mode" ) {
                auto [substmt, st] = _mode( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "prop" ) {
                auto [substmt, st] = _prop( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "cfg" ) {
                auto [substmt, st] = _cfg( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else if ( *id == "info" ) {
                auto [substmt, st] = _info( p );
                if ( st != parse_status::SUCCESS )
                        return { res, st };
                res.sub = std::move( substmt );
        } else {
                return { res, parse_status::UNKNOWN_CMD };
        }
        return { res, parse_status::SUCCESS };
}

// GEN END HERE

}  // namespace

vari::vval< stmt, invalid_stmt > parse( std::string_view inpt )
{
        parser::lexer  l{ inpt };
        parser::parser p{ std::move( l ) };

        auto [res, st] = _root( p );
        if ( st != parse_status::SUCCESS )
                return vari::vval< stmt, invalid_stmt >( invalid_stmt{ .st = st } );
        return vari::vval< stmt, invalid_stmt >( std::move( res ) );
}

}  // namespace servio::iface
