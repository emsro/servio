#pragma once

#include "hal.hpp"

#include <span>

namespace servio::plt
{

void cfg_store( uint32_t addr, std::span< std::byte > buffer );

}  // namespace servio::plt
