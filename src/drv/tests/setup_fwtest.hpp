#pragma once

#include "base/drv_interfaces.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>
#include <emlabcpp/experimental/testing/reactor.h>

namespace em = emlabcpp;

namespace servio::drv::tests
{

void setup_tests(
    em::pmr::memory_resource& mem_res,
    em::testing::reactor&     reac,
    em::testing::collector&   coll,
    base::clk_interface&      clk,
    em::result&               res );

}  // namespace servio::drv::tests
