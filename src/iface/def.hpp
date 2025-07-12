
#include "../lib/parser.hpp"
#include "./base.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <vari/vopt.h>
#include <vari/vval.h>

#pragma once

namespace em = emlabcpp;

namespace servio::iface
{

using parser::expr_tok;
using parser::string;

// GEN BEGIN HERE
enum class property
{
        mode,      // Mode of the servomotor
        current,   // Actual current flowing through the servomotor
        vcc,       // Actual input voltage
        temp,      // Actual temperature of the MCU
        position,  // Actual position of the servomotor
        velocity,  // Actual velocity of the servomotor
};

static constexpr std::array< property, 6 > property_values = {
    property::mode,
    property::current,
    property::vcc,
    property::temp,
    property::position,
    property::velocity,
};

static constexpr std::string_view to_str( property v )
{
        switch ( v ) {
        case property::mode:
                return "mode";
        case property::current:
                return "current";
        case property::vcc:
                return "vcc";
        case property::temp:
                return "temp";
        case property::position:
                return "position";
        case property::velocity:
                return "velocity";
        }
        return "unknown";
}

struct mode_disengaged_stmt
{
        friend constexpr auto
        operator<=>( mode_disengaged_stmt const&, mode_disengaged_stmt const& ) noexcept = default;
};

struct mode_power_stmt
{
        float power = 0;
        friend constexpr auto
        operator<=>( mode_power_stmt const&, mode_power_stmt const& ) noexcept = default;
};

struct mode_current_stmt
{
        float current = 0.0;
        friend constexpr auto
        operator<=>( mode_current_stmt const&, mode_current_stmt const& ) noexcept = default;
};

struct mode_velocity_stmt
{
        float velocity = 0.0;
        friend constexpr auto
        operator<=>( mode_velocity_stmt const&, mode_velocity_stmt const& ) noexcept = default;
};

struct mode_position_stmt
{
        float position = 0.0;
        friend constexpr auto
        operator<=>( mode_position_stmt const&, mode_position_stmt const& ) noexcept = default;
};

using mode_stmts = vari::typelist<
    mode_disengaged_stmt,
    mode_power_stmt,
    mode_current_stmt,
    mode_velocity_stmt,
    mode_position_stmt >;

struct mode_stmt
{
        vari::vval< mode_stmts > sub = mode_disengaged_stmt{};

        friend constexpr auto operator<=>( mode_stmt const&, mode_stmt const& ) noexcept = default;
};

struct prop_stmt
{
        property              name                                                       = {};
        friend constexpr auto operator<=>( prop_stmt const&, prop_stmt const& ) noexcept = default;
};

struct cfg_set_stmt
{
        string   field = {};
        expr_tok value = {};
        friend constexpr auto
        operator<=>( cfg_set_stmt const&, cfg_set_stmt const& ) noexcept = default;
};

struct cfg_get_stmt
{
        string field = {};
        friend constexpr auto
        operator<=>( cfg_get_stmt const&, cfg_get_stmt const& ) noexcept = default;
};

struct cfg_list5_stmt
{
        string  level  = "";
        int32_t offset = 0;
        int32_t n      = 5;
        friend constexpr auto
        operator<=>( cfg_list5_stmt const&, cfg_list5_stmt const& ) noexcept = default;
};

struct cfg_commit_stmt
{
        friend constexpr auto
        operator<=>( cfg_commit_stmt const&, cfg_commit_stmt const& ) noexcept = default;
};

struct cfg_clear_stmt
{
        friend constexpr auto
        operator<=>( cfg_clear_stmt const&, cfg_clear_stmt const& ) noexcept = default;
};

using cfg_stmts =
    vari::typelist< cfg_set_stmt, cfg_get_stmt, cfg_list5_stmt, cfg_commit_stmt, cfg_clear_stmt >;

struct cfg_stmt
{
        vari::vval< cfg_stmts > sub = cfg_set_stmt{};

        friend constexpr auto operator<=>( cfg_stmt const&, cfg_stmt const& ) noexcept = default;
};

struct info_stmt
{
        friend constexpr auto operator<=>( info_stmt const&, info_stmt const& ) noexcept = default;
};

using stmts = vari::typelist< mode_stmt, prop_stmt, cfg_stmt, info_stmt >;

struct stmt
{
        vari::vval< stmts > sub = mode_stmt{};

        friend constexpr auto operator<=>( stmt const&, stmt const& ) noexcept = default;
};
// GEN END HERE

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

struct invalid_stmt
{
        parse_status st;
};

vari::vval< stmt, invalid_stmt > parse( std::string_view inpt );

}  // namespace servio::iface
