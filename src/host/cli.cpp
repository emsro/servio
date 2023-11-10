
#include "host/cli.hpp"

namespace host
{

CLI::Option* verbose_opt( CLI::App& app, bool& val )
{
        return app.add_flag( "-v,--verbose", val, "verbosity" )->envname( "SERVIO_VERBOSE" );
}

CLI::Option* device_opt( CLI::App& app, std::filesystem::path& device )
{
        return app.add_option( "-d,--device", device, "device to use" )
            ->envname( "SERVIO_DEVICE" )
            ->check( CLI::ExistingFile );
}

CLI::Option* baudrate_opt( CLI::App& app, unsigned& baudrate )
{
        return app.add_option( "-b,--baudrate", baudrate, "baudrate for communication" )
            ->envname( "SERVIO_BAUDRATE" );
}

void common_cli::setup( CLI::App& app )
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

}  // namespace host
