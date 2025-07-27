
#include "../lib/parser.hpp"
#include "../status.hpp"
#include "./def.hpp"

namespace servio::iface
{

namespace
{
template < typename T >
struct _convert_type_impl;

template < typename T >
using _convert_type = _convert_type_impl< T >;

parse_status _load_val( arg_def& arg, vari::vref< parser::expr_t > val )
{
        return arg.val.visit(
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
}  // namespace

parse_status arg_parser::parse_args( std::span< arg_def > args ) &&
{

        opt< std::size_t > arg_i = 0;

        auto parse_arg = [&]( vari::vref< parser::expr_t > val ) -> parse_status {
                if ( !arg_i )
                        return parse_status::ARG_AFTER_KVAL;
                auto i = *arg_i;
                if ( i >= args.size() )
                        return parse_status::UNEXPECTED_ARG;
                *arg_i += 1;
                args[i].st = arg_status::FOUND;
                return _load_val( args[i], val );
        };

        while ( !p_.ended() ) {
                using R = parse_status;
                auto st = p_.next().visit(
                    [&]( vari::empty_t ) -> R {
                            return parse_status::SYNTAX_ERROR;
                    },
                    [&]( parser::op_t& ) -> R {
                            return parse_status::SYNTAX_ERROR;
                    },
                    [&]( parser::id_t& id ) -> R {
                            auto pp = p_;
                            auto op = pp.op();
                            if ( !op ) {
                                    string st{ id.s };
                                    return parse_arg( st );
                            }
                            p_ = pp;
                            arg_i.reset();
                            if ( *op != parser::op_t::ASSIGN )
                                    return parse_status::SYNTAX_ERROR;
                            auto e = p_.expr();
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

                            return _load_val( args[j], e.vref() );
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

static std::tuple< mode_disengaged_stmt, parse_status > _mode_disengaged( arg_parser ap )
{

        mode_disengaged_stmt     res;
        std::array< arg_def, 0 > arg_defs = {};
        parse_status             st       = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< mode_power_stmt, parse_status > _mode_power( arg_parser ap )
{

        mode_power_stmt          res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "power", .val = res.power } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< mode_current_stmt, parse_status > _mode_current( arg_parser ap )
{

        mode_current_stmt        res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "current", .val = res.current } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< mode_velocity_stmt, parse_status > _mode_velocity( arg_parser ap )
{

        mode_velocity_stmt       res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "velocity", .val = res.velocity } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< mode_position_stmt, parse_status > _mode_position( arg_parser ap )
{

        mode_position_stmt       res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "position", .val = res.position } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< mode_stmt, parse_status > _mode( cmd_parser p )
{
        mode_stmt    res;
        auto         id = p.next_cmd();
        parse_status st = parse_status::UNKNOWN_CMD;
        if ( !id )
                return { res, parse_status::CMD_MISSING };
        else if ( *id == "disengaged" )
                std::tie( res.sub, st ) = _mode_disengaged( std::move( p ) );
        else if ( *id == "power" )
                std::tie( res.sub, st ) = _mode_power( std::move( p ) );
        else if ( *id == "current" )
                std::tie( res.sub, st ) = _mode_current( std::move( p ) );
        else if ( *id == "velocity" )
                std::tie( res.sub, st ) = _mode_velocity( std::move( p ) );
        else if ( *id == "position" )
                std::tie( res.sub, st ) = _mode_position( std::move( p ) );
        else
                return { res, parse_status::UNKNOWN_CMD };
        return { res, st };
}

static std::tuple< prop_stmt, parse_status > _prop( arg_parser ap )
{
        _convert_type< property >::type name;
        prop_stmt                       res;
        std::array< arg_def, 1 >        arg_defs = {
            arg_def{ .st = arg_status::MISSING, .kw = "name", .val = name } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

        if ( st == parse_status::SUCCESS )
                st = _convert( name, res.name );

        return { res, st };
}

static std::tuple< cfg_set_stmt, parse_status > _cfg_set( arg_parser ap )
{

        cfg_set_stmt             res;
        std::array< arg_def, 2 > arg_defs = {
            arg_def{ .st = arg_status::MISSING, .kw = "field", .val = res.field },
            arg_def{ .st = arg_status::MISSING, .kw = "value", .val = res.value } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< cfg_get_stmt, parse_status > _cfg_get( arg_parser ap )
{

        cfg_get_stmt             res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::MISSING, .kw = "field", .val = res.field } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< cfg_list5_stmt, parse_status > _cfg_list5( arg_parser ap )
{

        cfg_list5_stmt           res;
        std::array< arg_def, 3 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "level", .val = res.level },
            arg_def{ .st = arg_status::DEFAULT, .kw = "offset", .val = res.offset },
            arg_def{ .st = arg_status::DEFAULT, .kw = "n", .val = res.n } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< cfg_commit_stmt, parse_status > _cfg_commit( arg_parser ap )
{

        cfg_commit_stmt          res;
        std::array< arg_def, 0 > arg_defs = {};
        parse_status             st       = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< cfg_clear_stmt, parse_status > _cfg_clear( arg_parser ap )
{

        cfg_clear_stmt           res;
        std::array< arg_def, 0 > arg_defs = {};
        parse_status             st       = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< cfg_stmt, parse_status > _cfg( cmd_parser p )
{
        cfg_stmt     res;
        auto         id = p.next_cmd();
        parse_status st = parse_status::UNKNOWN_CMD;
        if ( !id )
                return { res, parse_status::CMD_MISSING };
        else if ( *id == "set" )
                std::tie( res.sub, st ) = _cfg_set( std::move( p ) );
        else if ( *id == "get" )
                std::tie( res.sub, st ) = _cfg_get( std::move( p ) );
        else if ( *id == "list5" )
                std::tie( res.sub, st ) = _cfg_list5( std::move( p ) );
        else if ( *id == "commit" )
                std::tie( res.sub, st ) = _cfg_commit( std::move( p ) );
        else if ( *id == "clear" )
                std::tie( res.sub, st ) = _cfg_clear( std::move( p ) );
        else
                return { res, parse_status::UNKNOWN_CMD };
        return { res, st };
}

static std::tuple< info_stmt, parse_status > _info( arg_parser ap )
{

        info_stmt                res;
        std::array< arg_def, 0 > arg_defs = {};
        parse_status             st       = std::move( ap ).parse_args( arg_defs );

        return { res, st };
}

static std::tuple< stmt, parse_status > _root( cmd_parser p )
{
        stmt         res;
        auto         id = p.next_cmd();
        parse_status st = parse_status::UNKNOWN_CMD;
        if ( !id )
                return { res, parse_status::CMD_MISSING };
        else if ( *id == "mode" )
                std::tie( res.sub, st ) = _mode( std::move( p ) );
        else if ( *id == "prop" )
                std::tie( res.sub, st ) = _prop( std::move( p ) );
        else if ( *id == "cfg" )
                std::tie( res.sub, st ) = _cfg( std::move( p ) );
        else if ( *id == "info" )
                std::tie( res.sub, st ) = _info( std::move( p ) );
        else
                return { res, parse_status::UNKNOWN_CMD };
        return { res, st };
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
