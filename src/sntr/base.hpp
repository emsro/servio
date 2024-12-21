#pragma once

#include "../base/base.hpp"

#include <bitset>
#include <cstdint>
#include <variant>

namespace servio::sntr
{

static constexpr std::size_t max_eid        = 29;
static constexpr std::size_t terminated_eid = 30;
static constexpr std::size_t forbidden_eid  = 31;

static constexpr std::size_t eid_bits = std::max( terminated_eid, forbidden_eid ) + 1;
static_assert( max_eid < std::min( terminated_eid, forbidden_eid ) );

using ecode_set = std::bitset< eid_bits >;
using data_type = uint32_t;

}  // namespace servio::sntr
