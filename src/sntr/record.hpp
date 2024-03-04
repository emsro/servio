#pragma once

#include "sntr/base.hpp"

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
        record_state       st = record_state::UNSET;
        base::microseconds tp;
        const char*        src;
        ecode_set          ecodes;
        const char*        emsg;
        data_type          data;
};

}  // namespace servio::sntr
