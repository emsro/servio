#pragma once

#include <filesystem>
#include <optional>

namespace servio::ftester
{

struct config
{
        bool                                   verbose = false;
        std::filesystem::path                  c_device;
        std::filesystem::path                  d_device;
        std::optional< std::filesystem::path > output_dir = std::nullopt;
        bool                                   powerless  = false;

        std::optional< std::filesystem::path > firmware       = std::nullopt;
        std::optional< std::filesystem::path > openocd_config = std::nullopt;
};

config get_config( int argc, char* argv[] );

}  // namespace servio::ftester
