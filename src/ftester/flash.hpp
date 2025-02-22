#pragma once

#include <filesystem>
#include <joque/task.hpp>
#include <optional>

namespace servio::ftester
{
template < typename T >
using opt = std::optional< T >;

joque::task make_openocd_flash_task(
    std::filesystem::path const&        firmware,
    std::filesystem::path const&        ocd_cfg,
    opt< std::filesystem::path > const& log_file,
    joque::resource const&              dev_res );

joque::task make_bmp_flash_task(
    std::filesystem::path const& firmware,
    std::filesystem::path const& device,
    joque::resource const&       dev_res );

void flash_firmware_openocd(
    std::filesystem::path const& firmware,
    std::filesystem::path const& ocdconf );

}  // namespace servio::ftester
