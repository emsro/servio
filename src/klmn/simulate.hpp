#pragma once

#include "./kalman.hpp"

namespace servio::klmn
{

std::vector< state > simulate(
    sec_time                          tdiff,
    float                             process_deviation,
    float                             observation_deviation,
    std::vector< observation > const& observations,
    state_range                       sr );

}  // namespace servio::klmn
