#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace servio::ftester
{

template < typename T >
using opt = std::optional< T >;

struct config
{
        opt< std::filesystem::path > input      = std::nullopt;
        bool                         powerless  = false;
        opt< std::filesystem::path > output_dir = std::nullopt;
        std::string                  filter;

        std::filesystem::path c_device   = "/dev/ttyUSB0";
        uint32_t              c_baudrate = 460800;
        std::filesystem::path d_device   = "/dev/ttyACM0";
        uint32_t              d_baudrate = 460800;
        opt< bool >           verbose    = false;

        opt< std::filesystem::path > firmware       = std::nullopt;
        opt< std::filesystem::path > openocd_config = std::nullopt;
};

config get_config( int argc, char* argv[] );

}  // namespace servio::ftester
