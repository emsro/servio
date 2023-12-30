#pragma once

#include "hal.hpp"

namespace servio::plt
{

void cfg_erase( uint32_t start_addr );

bool cfg_write( uint32_t addr, uint64_t val );

}  // namespace servio::plt
