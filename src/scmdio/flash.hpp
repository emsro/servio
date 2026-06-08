#pragma once

#include "base.hpp"
#include "port.hpp"

#include <filesystem>

namespace servio::scmdio
{

// High-level flash command:
//   1. Try ROM bootloader INIT (100 ms timeout).
//   2. If no response, send "dfu" command to running firmware, wait 200 ms, retry INIT.
//   3. Upload the binary, then send GO to 0x08000000 so firmware starts immediately.
awaitable< void > flash_firmware(
    io_context&                  io_ctx,
    std::filesystem::path const& port_path,
    std::filesystem::path const& file_path,
    unsigned                     baudrate );

}  // namespace servio::scmdio
