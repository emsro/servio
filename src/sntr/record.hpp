#pragma once

#include "./base.hpp"

namespace servio::sntr
{

enum class record_state
{
        LOCKED,
        SET,
        UNSET,
};

struct record
{
        record_state st;
        microseconds tp;
        char const*  src;
        uint32_t     ecodes;
        char const*  emsg;
        data_type    data;
};

static_assert( std::is_trivially_default_constructible< record >::value );

constexpr record default_record()
{
        return {
            .st     = record_state::UNSET,
            .tp     = 0_ms,
            .src    = nullptr,
            .ecodes = {},
            .emsg   = nullptr,
            .data   = {},
        };
}

}  // namespace servio::sntr
