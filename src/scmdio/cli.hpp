

#include "../base/base.hpp"
#include "./port.hpp"

#include <CLI/CLI.hpp>

#pragma once

namespace servio::scmdio
{

CLI::Option* verbose_opt( CLI::App& app, opt< bool >& val );

CLI::Option* coms_opt( CLI::App& app, std::filesystem::path& device );
CLI::Option* dcoms_opt( CLI::App& app, std::filesystem::path& device );

CLI::Option* baudrate_opt( CLI::App& app, unsigned& baudrate );

CLI::Option* powerless_flag( CLI::App& app, bool& is_powerless );

struct port_cli
{
        std::filesystem::path device   = "/dev/ttyUSB0";
        unsigned              baudrate = 460800;
};

struct cobs_cli : port_cli
{
        sptr< cobs_port > get( io_context& ctx )
        {
                if ( !prt )
                        prt = std::make_shared< cobs_port >( ctx, device, baudrate );
                return prt;
        }

private:
        sptr< cobs_port > prt = nullptr;
};

struct serial_cli : port_cli
{
        sptr< serial_stream > get( io_context& ctx )
        {
                if ( !prt )
                        prt = std::make_shared< serial_stream >( ctx, device, baudrate );
                return prt;
        }

private:
        sptr< serial_stream > prt = nullptr;
};

void port_opts( CLI::App& app, port_cli& cli );

}  // namespace servio::scmdio
