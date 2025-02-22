#pragma once

#include "./cli.hpp"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vari/uvptr.h>

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
        uint32_t              c_baudrate = 230400;
        std::filesystem::path d_device   = "/dev/ttyACM0";
        uint32_t              d_baudrate = 230400;
        opt< bool >           verbose    = false;

        vari::uvptr< openocd_flash_config, bmp_config > flash_cmd;
};

config get_config( int argc, char* argv[] );

}  // namespace servio::ftester
