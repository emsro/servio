#pragma once

#include "../core/core.hpp"
#include "../core/drivers.hpp"
#include "./utest.hpp"

namespace em = emlabcpp;

namespace servio::ftest
{

void setup_tests(
    asrt::task_ctx& ctx,
    asrt_reac_assm& assm,
    core::drivers&  cdrv,
    core::core&     core,
    status&         res );

};
