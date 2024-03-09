#pragma once

#include "drv/interfaces.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>
#include <emlabcpp/experimental/testing/parameters.h>
#include <emlabcpp/experimental/testing/reactor.h>

namespace em = emlabcpp;

namespace servio::drv::tests
{

void setup_tests(
    em::pmr::memory_resource& mem,
    em::testing::reactor&     reac,
    em::testing::collector&   coll,
    em::testing::parameters&  params,
    clk_interface&            clk,
    com_interface&            comms,
    period_interface&         period,
    pwm_motor_interface&      pwm,
    em::result&               res );

}  // namespace servio::drv::tests
