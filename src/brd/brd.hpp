#pragma once

#include "../cfg/map.hpp"
#include "../core/drivers.hpp"

namespace servio::brd
{
/// An API that has to be implemented by each board variation.

/// Initial setup call for the board
em::result setup_board();

/// Initialize all core drivers for the servo
core::drivers setup_core_drivers();

/// Initialize debug interface
drv::com_iface* setup_debug_comms();

}  // namespace servio::brd
