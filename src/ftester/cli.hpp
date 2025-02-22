#pragma once

#include <CLI/CLI.hpp>
#include <filesystem>
#include <optional>
#include <vari/uvptr.h>

namespace servio::ftester
{

template < typename T >
using opt = std::optional< T >;

struct openocd_flash_config
{
        std::filesystem::path openocd_config;
        std::filesystem::path fw;
};

struct bmp_config
{
        std::filesystem::path device;
        std::filesystem::path fw;
};

void flash_commands( CLI::App& app, vari::uvptr< openocd_flash_config, bmp_config >& flash_cmd );

}  // namespace servio::ftester
