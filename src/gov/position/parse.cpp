
#include "../../iface/base.hpp"
#include "./iface.hpp"

namespace servio::gov::pos::iface
{

using servio::iface::arg_def;
using servio::iface::arg_parser;
using servio::iface::arg_status;
using servio::iface::cmd_parser;
using servio::iface::parse_status;

namespace
{
// GEN BEGIN HERE

static std::tuple< set_stmt, parse_status > _set( arg_parser ap )
{

        set_stmt                 res;
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::MISSING, .kw = "goal", .val = res.goal } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

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
        std::array< arg_def, 1 > arg_defs = {
            arg_def{ .st = arg_status::DEFAULT, .kw = "offset", .val = res.offset } };
        parse_status st = std::move( ap ).parse_args( arg_defs );

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
        else
                return { res, parse_status::UNKNOWN_CMD };
        return { res, st };
}

static std::tuple< stmt, parse_status > _root( cmd_parser p )
{
        stmt         res;
        auto         id = p.next_cmd();
        parse_status st = parse_status::UNKNOWN_CMD;
        if ( !id )
                return { res, parse_status::CMD_MISSING };
        else if ( *id == "set" )
                std::tie( res.sub, st ) = _set( std::move( p ) );
        else if ( *id == "cfg" )
                std::tie( res.sub, st ) = _cfg( std::move( p ) );
        else
                return { res, parse_status::UNKNOWN_CMD };
        return { res, st };
}

// GEN END HERE
}  // namespace

std::tuple< stmt, parse_status > parse_curr( cmd_parser p )
{
        return _root( std::move( p ) );
}

}  // namespace servio::gov::pos::iface
