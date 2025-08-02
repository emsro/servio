#pragma once

#include <cstdint>
//
#include "../lib/json_ser.hpp"
#include "../lib/parser.hpp"

#include <vari/bits/typelist.h>

namespace servio::iface
{

template < typename T >
using opt = std::optional< T >;

using parser::expr_tok;
using parser::string;

using value_type = vari::typelist< string, float, int32_t >;

enum class parse_status : uint8_t
{
        SUCCESS,
        SYNTAX_ERROR,       // syntax error in the input
        ARG_TYPE_MISMATCH,  // type mismatch between argument and value
        ARG_DUPLICATE,      // duplicate argument
        ARG_AFTER_KVAL,     // argument appears after key-value pair
        ARG_MISSING,        // required argument is missing
        CMD_MISSING,        // command is missing
        UNKNOWN_ARG_KEY,    // unknown key argument
        UNKNOWN_CMD,        // unknown command
        UNKNOWN_VALUE,      // unknown value for an argument
        UNEXPECTED_ARG,     // unexpected argument in the input
};

enum class arg_status : uint8_t
{
        DEFAULT,
        MISSING,
        FOUND
};

struct arg_def
{
        arg_status                         st = arg_status::MISSING;
        char const*                        kw;
        vari::vref< value_type, expr_tok > val;
};

struct arg_parser
{
        arg_parser( parser::parser& p )
          : p_( p )
        {
        }

        parse_status parse_args( std::span< arg_def > args ) &&;

private:
        parser::parser& p_;
};

struct cmd_parser
{
        cmd_parser( parser::parser& p )
          : p_( p )
        {
        }

        cmd_parser( cmd_parser const& )            = delete;
        cmd_parser& operator=( cmd_parser const& ) = delete;
        cmd_parser( cmd_parser&& )                 = default;

        opt< std::string_view > next_cmd() const
        {
                return p_.id();
        }

        operator arg_parser() &&
        {
                return arg_parser{ p_ };
        }

private:
        parser::parser& p_;
};

struct invalid_stmt
{
        parse_status st;
};

inline std::string_view to_str( parse_status st )
{
        switch ( st ) {
        case parse_status::SUCCESS:
                return "success";
        case parse_status::SYNTAX_ERROR:
                return "syntax error";
        case parse_status::ARG_TYPE_MISMATCH:
                return "argument type mismatch";
        case parse_status::ARG_DUPLICATE:
                return "duplicate argument";
        case parse_status::ARG_AFTER_KVAL:
                return "argument after key-value pair";
        case parse_status::ARG_MISSING:
                return "argument missing";
        case parse_status::CMD_MISSING:
                return "command missing";
        case parse_status::UNKNOWN_ARG_KEY:
                return "unknown argument key";
        case parse_status::UNKNOWN_CMD:
                return "unknown command";
        case parse_status::UNKNOWN_VALUE:
                return "unknown value for an argument";
        case parse_status::UNEXPECTED_ARG:
                return "unexpected argument";
        }
        return "unknown status";
}

struct root_ser
{
        root_ser( json::jval_ser& as ) noexcept
          : as( as )
        {
        }

        root_ser( root_ser const& )            = delete;
        root_ser& operator=( root_ser const& ) = delete;
        root_ser( root_ser&& ) noexcept        = default;

        json::array_ser ok() && noexcept
        {
                as( "OK" );
                return std::move( as );
        }

        json::array_ser nok() && noexcept
        {
                as( "OK" );
                return std::move( as );
        }

private:
        json::array_ser as;
};

}  // namespace servio::iface
