#include "./cli.hpp"

#include <vari/uvref.h>

namespace servio::ftester
{

namespace
{

CLI::Option* firmware_opt( CLI::App& app, std::filesystem::path& fw )
{
        return app.add_option( "firmware", fw, "Firmware for the device" )
            ->envname( "SERVIO_FIRMWARE" )
            ->check( CLI::ExistingFile );
}
}  // namespace

void flash_commands( CLI::App& app, vari::uvptr< openocd_flash_config, bmp_config >& flash_cmd )
{
        auto  ocd_data = std::make_shared< openocd_flash_config >();
        auto* ocd      = app.add_subcommand( "openocd", "Flash using openocd" );
        firmware_opt( *ocd, ocd_data->fw );
        ocd->add_option(
               "config",
               ocd_data->openocd_config,
               "Config file for openocd used to flash the device" )
            ->envname( "SERVIO_OPENOCD_CONFIG" )
            ->check( CLI::ExistingFile );
        ocd->final_callback( [ocd_data, &flash_cmd]() {
                flash_cmd = vari::uwrap( std::move( *ocd_data ) ).vptr();
        } );

        auto  bmp_data = std::make_shared< bmp_config >();
        auto* bmp      = app.add_subcommand( "bmp", "Flash using Black Magic Probe" );
        firmware_opt( *bmp, bmp_data->fw );
        bmp->add_option( "bmp", bmp_data->device, "Device to flash" )
            ->envname( "SERVIO_BMP_DEVICE" )
            ->check( CLI::ExistingFile );
        bmp->final_callback( [bmp_data, &flash_cmd]() {
                flash_cmd = vari::uwrap( std::move( *bmp_data ) ).vptr();
        } );
        bmp->excludes( ocd );
}

}  // namespace servio::ftester
