#pragma once

#include "hal.hpp"

#include <span>

namespace servio::plt
{

void cfg_erase( uint32_t start_addr );

bool cfg_write( uint32_t addr, std::span< std::byte > val );

}  // namespace servio::plt
