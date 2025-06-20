
#include "./config.hpp"

#include "../scmdio/cli.hpp"
#include "./cli.hpp"

namespace servio::ftester
{

config get_config( int argc, char* argv[] )
{
        config   cfg;
        CLI::App app( "Utility to run basic input-less tests" );

        scmdio::verbose_opt( app );
        scmdio::coms_opt( app, cfg.c_device );
        scmdio::dcoms_opt( app, cfg.d_device );
        app.add_option( "-o,--output", cfg.output_dir, "Directory to use for storing results" );
        app.add_option( "--input", cfg.input, "Input data" );
        flash_commands( app, cfg.flash_cmd );
        app.add_option( "--filter", cfg.filter, "Filter for tests" );

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
