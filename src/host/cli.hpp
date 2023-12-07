

#include "host/async_cobs.hpp"

#include <CLI/CLI.hpp>

#pragma once

namespace host
{

CLI::Option* verbose_opt( CLI::App& app, bool& val );

CLI::Option* device_opt( CLI::App& app, std::filesystem::path& device );

CLI::Option* baudrate_opt( CLI::App& app, unsigned& baudrate );

CLI::Option* powerless_flag( CLI::App& app, bool& is_powerless );

struct common_cli
{

        std::filesystem::path        device   = "/dev/ttyUSB0";
        unsigned                     baudrate = 115200;
        boost::asio::io_context      context{};
        std::unique_ptr< cobs_port > port_ptr{};

        void setup( CLI::App& app );
};

}  // namespace host
