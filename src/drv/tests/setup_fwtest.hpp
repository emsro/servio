#pragma once

#include "base/drv_interfaces.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>
#include <emlabcpp/experimental/testing/parameters.h>
#include <emlabcpp/experimental/testing/reactor.h>

namespace em = emlabcpp;

namespace servio::drv::tests
{

void setup_tests(
    em::pmr::memory_resource&  mem,
    em::testing::reactor&      reac,
    em::testing::collector&    coll,
    em::testing::parameters&   params,
    base::clk_interface&       clk,
    base::com_interface&       comms,
    base::period_interface&    period,
    base::pwm_motor_interface& pwm,
    em::result&                res );

}  // namespace servio::drv::tests
