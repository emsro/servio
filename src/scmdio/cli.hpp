

#include "../base/base.hpp"
#include "./async_cobs.hpp"

#include <CLI/CLI.hpp>

#pragma once

namespace servio::scmdio
{

CLI::Option* verbose_opt( CLI::App& app, opt< bool >& val );

CLI::Option* coms_opt( CLI::App& app, std::filesystem::path& device );
CLI::Option* dcoms_opt( CLI::App& app, std::filesystem::path& device );

CLI::Option* baudrate_opt( CLI::App& app, unsigned& baudrate );

CLI::Option* powerless_flag( CLI::App& app, bool& is_powerless );

struct common_cli
{

        std::filesystem::path        device   = "/dev/ttyUSB0";
        unsigned                     baudrate = 460800;
        boost::asio::io_context      context{};
        std::unique_ptr< cobs_port > port_ptr{};

        void setup( CLI::App& app );
};

}  // namespace servio::scmdio
