#include "host/async_cobs.hpp"

#include <CLI/CLI.hpp>
#include <boost/asio.hpp>

#pragma once

namespace host
{

inline CLI::Option* device_opt( CLI::App& app, std::filesystem::path& device )
{
        return app.add_option( "-d,--device", device, "device to use" )
            ->envname( "SERVIO_DEVICE" )
            ->check( CLI::ExistingFile );
}

inline CLI::Option* baudrate_opt( CLI::App& app, unsigned& baudrate )
{
        return app.add_option( "-b,--baudrate", baudrate, "baudrate for communication" )
            ->envname( "SERVIO_BAUDRATE" );
}

struct common_cli
{

        std::filesystem::path        device   = "/dev/ttyUSB0";
        unsigned                     baudrate = 115200;
        boost::asio::io_context      context{};
        std::unique_ptr< cobs_port > port_ptr{};

        void setup( CLI::App& app )
        {
                device_opt( app, device )->capture_default_str();
                baudrate_opt( app, baudrate )->capture_default_str();
                app.parse_complete_callback( [&] {
                        port_ptr = std::make_unique< cobs_port >( context, device, baudrate );
                        const int res =
                            ::tcflush( port_ptr->port.lowest_layer().native_handle(), TCIOFLUSH );
                        if ( res != 0 ) {
                                std::cerr << "Failed to flush serial buffer" << std::endl;
                        }
                } );
        }
};

}  // namespace host