#pragma once

#include "base/base.hpp"

#include <bitset>
#include <cstdint>
#include <variant>

namespace servio::sntr
{

static constexpr std::size_t max_eid = 31;

using ecode_set = std::bitset< max_eid + 1 >;
using data_type = std::variant< std::monostate, uint32_t, std::bitset< 32 > >;

}  // namespace servio::sntr
