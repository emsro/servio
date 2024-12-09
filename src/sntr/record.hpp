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
        record_state st = record_state::UNSET;
        microseconds tp{ 0 };
        char const*  src = nullptr;
        ecode_set    ecodes;
        char const*  emsg = nullptr;
        data_type    data;
};

}  // namespace servio::sntr
