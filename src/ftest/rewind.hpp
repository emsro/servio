#pragma once

#include "core/core.hpp"
#include "core/drivers.hpp"

namespace servio::ftest
{

namespace em = emlabcpp;

void rewind(
    core::core&              cor,
    drv::clk_interface&      iclk,
    drv::position_interface& ipos,
    base::microseconds       timeout,
    em::min_max< float >     area,
    float                    current );

}  // namespace servio::ftest
