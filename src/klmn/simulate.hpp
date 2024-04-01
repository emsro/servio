#pragma once

#include "klmn/kalman.hpp"

namespace servio::klmn
{

std::vector< state > simulate(
    sec_time                          tdiff,
    float                             process_deviation,
    float                             observation_deviation,
    const std::vector< observation >& observations,
    state_range                       sr );

}  // namespace servio::klmn
