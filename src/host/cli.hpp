#include <CLI/CLI.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace host
{

struct common_cli
{

        std::filesystem::path                       device   = "/dev/ttyUSB0";
        unsigned                                    baudrate = 115200;
        boost::asio::io_context                     context{};
        std::unique_ptr< boost::asio::serial_port > port_ptr{};

        void setup( CLI::App& app )
        {
                app.add_option( "-d,--device", device, "device to use" )
                    ->envname( "SERVIO_DEVICE" )
                    ->check( CLI::ExistingFile )
                    ->capture_default_str();
                app.add_option( "-b,--baudrate", baudrate, "baudrate for communication" )
                    ->envname( "SERVIO_BAUDRATE" )
                    ->capture_default_str();
                app.parse_complete_callback( [&] {
                        port_ptr = std::make_unique< boost::asio::serial_port >(
                            context, device, baudrate );
                } );
        }
};

}  // namespace host
