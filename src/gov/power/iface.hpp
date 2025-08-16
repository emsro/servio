#pragma once

#include "../../iface/base.hpp"

namespace servio::gov::pow::iface
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

using stmts = vari::typelist< set_stmt >;

struct stmt
{
        vari::vval< stmts > sub = set_stmt{};

        friend constexpr auto operator<=>( stmt const&, stmt const& ) noexcept = default;
};

// GEN END HERE
std::tuple< iface::stmt, iface::parse_status > parse_vel( iface::cmd_parser p );
}  // namespace servio::gov::pow::iface
