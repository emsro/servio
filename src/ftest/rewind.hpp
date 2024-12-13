#pragma once

#include "../core/core.hpp"
#include "../core/drivers.hpp"

namespace servio::ftest
{

namespace em = emlabcpp;

void rewind(
    core::core&          cor,
    drv::clk_iface&      iclk,
    drv::pos_iface&      ipos,
    microseconds         timeout,
    em::min_max< float > area,
    float                current );

}  // namespace servio::ftest
