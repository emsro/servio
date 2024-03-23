
#include "ftester/config.hpp"

#include "ftester/cli.hpp"
#include "scmdio/cli.hpp"

namespace servio::ftester
{

config get_config( int argc, char* argv[] )
{
        config   cfg;
        CLI::App app( "Utility to run basic input-less tests" );

        scmdio::verbose_opt( app, cfg.verbose );
        scmdio::coms_opt( app, cfg.c_device );
        scmdio::dcoms_opt( app, cfg.d_device );
        app.add_option( "-o,--output", cfg.output_dir, "Directory to use for storing results" );
        app.add_option( "--input", cfg.input, "Input data" );
        scmdio::powerless_flag( app, cfg.powerless );
        CLI::Option* ocfg = ocfg_opt( app, cfg.openocd_config );
        firmware_opt( app, cfg.firmware, ocfg );
        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                app.exit( e );
                std::abort();
        }

        return cfg;
}

}  // namespace servio::ftester
