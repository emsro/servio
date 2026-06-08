
#include "../lib/parser.hpp"
#include "./base.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <vari/vopt.h>
#include <vari/vval.h>

#pragma once

namespace em = emlabcpp;

namespace servio::iface
{

// GEN BEGIN HERE
enum class property
{
        current,   // Actual current flowing through the servomotor
        vcc,       // Actual input voltage
        temp,      // Actual temperature of the MCU
        position,  // Actual position of the servomotor
        velocity,  // Actual velocity of the servomotor
};

static constexpr std::array< property, 5 > property_values = {
    property::current,
    property::vcc,
    property::temp,
    property::position,
    property::velocity,
};

static constexpr std::string_view to_str( property v )
{
        switch ( v ) {
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

struct govctl_activate_stmt
{
        string governor = {};
        friend constexpr auto
        operator<=>( govctl_activate_stmt const&, govctl_activate_stmt const& ) noexcept = default;
};

struct govctl_deactivate_stmt
{
        friend constexpr auto operator<=>(
            govctl_deactivate_stmt const&,
            govctl_deactivate_stmt const& ) noexcept = default;
};

struct govctl_active_stmt
{
        friend constexpr auto
        operator<=>( govctl_active_stmt const&, govctl_active_stmt const& ) noexcept = default;
};

struct govctl_list_stmt
{
        int32_t index = {};
        friend constexpr auto
        operator<=>( govctl_list_stmt const&, govctl_list_stmt const& ) noexcept = default;
};

using govctl_stmts = vari::
    typelist< govctl_activate_stmt, govctl_deactivate_stmt, govctl_active_stmt, govctl_list_stmt >;

struct govctl_stmt
{
        vari::vval< govctl_stmts > sub = govctl_activate_stmt{};
        friend constexpr auto
        operator<=>( govctl_stmt const&, govctl_stmt const& ) noexcept = default;
};

struct gov_stmt
{
        cmd_parser            parser;
        friend constexpr auto operator<=>( gov_stmt const&, gov_stmt const& ) noexcept = default;
};

struct prop_stmt
{
        property              name                                                       = {};
        friend constexpr auto operator<=>( prop_stmt const&, prop_stmt const& ) noexcept = default;
};

struct cfg_set_stmt
{
        string   field    = {};
        expr_tok value    = {};
        string   governor = "";
        friend constexpr auto
        operator<=>( cfg_set_stmt const&, cfg_set_stmt const& ) noexcept = default;
};

struct cfg_get_stmt
{
        string field    = {};
        string governor = "";
        friend constexpr auto
        operator<=>( cfg_get_stmt const&, cfg_get_stmt const& ) noexcept = default;
};

struct cfg_list_stmt
{
        int32_t index    = 0;
        string  governor = "";
        friend constexpr auto
        operator<=>( cfg_list_stmt const&, cfg_list_stmt const& ) noexcept = default;
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
    vari::typelist< cfg_set_stmt, cfg_get_stmt, cfg_list_stmt, cfg_commit_stmt, cfg_clear_stmt >;

struct cfg_stmt
{
        vari::vval< cfg_stmts > sub = cfg_set_stmt{};
        friend constexpr auto   operator<=>( cfg_stmt const&, cfg_stmt const& ) noexcept = default;
};

struct info_stmt
{
        friend constexpr auto operator<=>( info_stmt const&, info_stmt const& ) noexcept = default;
};

using stmts = vari::typelist< govctl_stmt, gov_stmt, prop_stmt, cfg_stmt, info_stmt >;

struct stmt
{
        vari::vval< stmts >   sub                                              = govctl_stmt{};
        friend constexpr auto operator<=>( stmt const&, stmt const& ) noexcept = default;
};

// GEN END HERE

vari::vval< stmt, invalid_stmt > parse( parser::parser& p );

}  // namespace servio::iface
