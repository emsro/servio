
#include "./cli.hpp"

namespace servio::scmdio
{

CLI::Option* verbose_opt( CLI::App& app, opt< bool >& val )
{
        return app.add_flag( "-v,--verbose", val, "verbosity" )
            ->envname( "SERVIO_VERBOSE" )
            ->each( []( std::string const& ) {
                    em::DEBUG_LOGGER.set_option( em::set_stdout( true ) );
            } );
}

CLI::Option* coms_opt( CLI::App& app, std::filesystem::path& comms )
{
        return app.add_option( "-c,--comms", comms, "device for communication" )
            ->envname( "SERVIO_COMMS" )
            ->check( CLI::ExistingFile );
}

CLI::Option* dcoms_opt( CLI::App& app, std::filesystem::path& comms )
{
        return app.add_option( "-d,--dcomms", comms, "device for debug communication" )
            ->envname( "SERVIO_DCOMMS" )
            ->check( CLI::ExistingFile );
}

CLI::Option* baudrate_opt( CLI::App& app, unsigned& baudrate )
{
        return app.add_option( "-b,--baudrate", baudrate, "baudrate for communication" )
            ->envname( "SERVIO_BAUDRATE" );
}

CLI::Option* powerless_flag( CLI::App& app, bool& is_powerless )
{
        return app.add_flag(
            "--powerless,!--power",
            is_powerless,
            "If enabled the tests will get info that no power is present" );
}

void common_cli::setup( CLI::App& app )
{
        coms_opt( app, device )->capture_default_str();
        baudrate_opt( app, baudrate )->capture_default_str();
        app.parse_complete_callback( [&] {
                port_ptr = std::make_unique< cobs_port >( context, device, baudrate );
                const int res =
                    ::tcflush( port_ptr->port.lowest_layer().native_handle(), TCIOFLUSH );
                if ( res != 0 )
                        std::cerr << "Failed to flush serial buffer" << std::endl;
        } );
}

}  // namespace servio::scmdio
