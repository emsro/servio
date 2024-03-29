#pragma once

#include "cfg/map.hpp"
#include "core/drivers.hpp"

namespace servio::brd
{
/// An API that has to be implemented by each board variation.

using page = em::view< std::byte* >;

/// Pages that shall be used for persistent configuration
em::view< const page* > get_persistent_pages();

/// Returns default configuration for servo for the board
cfg::map get_default_config();

/// Initial setup call for the board
em::result setup_board();

/// Initialize all core drivers for the servo
core::drivers setup_core_drivers();

/// Initialize debug interface
drv::com_iface* setup_debug_comms();

}  // namespace servio::brd
