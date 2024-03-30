#pragma once

#include <filesystem>
#include <optional>

namespace servio::ftester
{

struct config
{
        std::optional< std::filesystem::path > input      = std::nullopt;
        bool                                   powerless  = false;
        std::optional< std::filesystem::path > output_dir = std::nullopt;
        std::string                            filter;

        std::filesystem::path c_device   = "/dev/ttyUSB0";
        uint32_t              c_baudrate = 460800;
        std::filesystem::path d_device   = "/dev/ttyACM0";
        uint32_t              d_baudrate = 460800;
        std::optional< bool > verbose    = false;

        std::optional< std::filesystem::path > firmware       = std::nullopt;
        std::optional< std::filesystem::path > openocd_config = std::nullopt;
};

config get_config( int argc, char* argv[] );

}  // namespace servio::ftester
