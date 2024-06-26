#pragma once

#include <filesystem>
#include <joque/task.hpp>
#include <optional>

namespace servio::ftester
{

joque::task make_flash_task(
    const std::filesystem::path&                  firmware,
    const std::filesystem::path&                  ocd_cfg,
    const std::optional< std::filesystem::path >& log_file,
    const joque::resource&                        dev_res );

void flash_firmware( const std::filesystem::path& firmware, const std::filesystem::path& ocdconf );

}  // namespace servio::ftester
