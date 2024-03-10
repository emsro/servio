#pragma once

#include <filesystem>

namespace servio::ftester
{

void flash_firmware( const std::filesystem::path& firmware, const std::filesystem::path& ocdconf );

}  // namespace servio::ftester
