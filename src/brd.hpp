#pragma once

#include "config.hpp"
#include "core_drivers.hpp"

namespace brd
{
/// An API that has to be implemented by each board variation.

/// Pages that shall be used for persistent configuration
em::view< const em::view< std::byte* >* > get_persistent_pages();

/// Returns default configuration for servo for the board
cfg_map get_default_config();

/// Initial setup call for the board
void setup_board();

/// Initialize all core drivers for the servo
core_drivers setup_core_drivers();

/// Initialize debug interface
com_interface* setup_debug_comms();

}  // namespace brd
