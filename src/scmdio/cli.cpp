
#include "./cli.hpp"

namespace servio::scmdio
{

CLI::Option* verbose_opt( CLI::App& app )
{
        return app
            .add_flag(
                "-v,--verbose",
                [&]( std::int64_t x ) {
                        if ( x == 0 )
                                spdlog::set_level( spdlog::level::info );
                        else if ( x == 1 )
                                spdlog::set_level( spdlog::level::debug );
                        else if ( x == 2 )
                                spdlog::set_level( spdlog::level::trace );
                        else
                                throw CLI::ValidationError( "Invalid verbosity level" );
                },
                "verbosity" )
            ->envname( "SERVIO_VERBOSE" );
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

void port_opts( CLI::App& app, port_cli& cli )
{
        coms_opt( app, cli.device )->capture_default_str();
        baudrate_opt( app, cli.baudrate )->capture_default_str();
}

}  // namespace servio::scmdio
