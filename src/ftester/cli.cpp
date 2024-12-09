#include "./cli.hpp"

namespace servio::ftester
{

CLI::Option* ocfg_opt( CLI::App& app, opt< std::filesystem::path >& o_cfg )
{
        return app
            .add_option(
                "--ocd,--config", o_cfg, "Config file for openocd used to flash the device" )
            ->envname( "SERVIO_OPENOCD_CONFIG" )
            ->check( CLI::ExistingFile );
}

CLI::Option* firmware_opt( CLI::App& app, opt< std::filesystem::path >& fw, CLI::Option* cfg_opt )
{
        return app.add_option( "-f,--firmware", fw, "Firmware for the device" )
            ->envname( "SERVIO_FIRMWARE" )
            ->needs( cfg_opt )
            ->check( CLI::ExistingFile );
}

}  // namespace servio::ftester
