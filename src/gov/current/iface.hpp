#pragma once

#include "../../iface/base.hpp"

namespace servio::gov::curr::iface
{
using servio::iface::cmd_parser;
using servio::iface::expr_tok;
using servio::iface::invalid_stmt;
using servio::iface::parse_status;
using servio::iface::string;

// GEN BEGIN HERE
struct set_stmt
{
        float                 goal                                                     = {};
        friend constexpr auto operator<=>( set_stmt const&, set_stmt const& ) noexcept = default;
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
        int32_t offset = 0;
        friend constexpr auto
        operator<=>( cfg_list5_stmt const&, cfg_list5_stmt const& ) noexcept = default;
};

using cfg_stmts = vari::typelist< cfg_set_stmt, cfg_get_stmt, cfg_list5_stmt >;

struct cfg_stmt
{
        vari::vval< cfg_stmts > sub = cfg_set_stmt{};

        friend constexpr auto operator<=>( cfg_stmt const&, cfg_stmt const& ) noexcept = default;
};

using stmts = vari::typelist< set_stmt, cfg_stmt >;

struct stmt
{
        vari::vval< stmts > sub = set_stmt{};

        friend constexpr auto operator<=>( stmt const&, stmt const& ) noexcept = default;
};

// GEN END HERE
}  // namespace servio::gov::curr::iface
