
#include "../../iface/base.hpp"
#include "./iface.hpp"

namespace servio::gov::pow::iface
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

        return { std::move( res ), st };
}

static std::tuple< stmt, parse_status > _root( cmd_parser p )
{
        stmt         res;
        auto         id = p.next_cmd();
        parse_status st = parse_status::UNKNOWN_CMD;
        if ( !id )
                return { std::move( res ), parse_status::CMD_MISSING };
        else if ( *id == "set" )
                std::tie( res.sub, st ) = _set( std::move( p ) );
        else
                return { std::move( res ), parse_status::UNKNOWN_CMD };
        return { std::move( res ), st };
}

// GEN END HERE
}  // namespace

std::tuple< stmt, parse_status > parse_pow( cmd_parser p )
{
        return _root( std::move( p ) );
}

}  // namespace servio::gov::pow::iface
