#include "./flash.hpp"

#include <emlabcpp/experimental/logging.h>
#include <joque/process.hpp>
#include <thread>

namespace servio::ftester
{

joque::task make_openocd_flash_task(
    std::filesystem::path const&        firmware,
    std::filesystem::path const&        ocd_cfg,
    opt< std::filesystem::path > const& log_file,
    joque::resource const&              dev_res )
{
        auto f = [&]( auto&&... extra ) {
                return joque::task{
                    .job = joque::process::derive(
                               "openocd",
                               "-f",
                               ocd_cfg,
                               extra...,
                               "-c",
                               "program " + firmware.string() + " verify reset exit" )
                               .add_input( firmware ),
                    .resources = { dev_res },
                };
        };
        if ( log_file.has_value() )
                return f( "-l", joque::out( *log_file ) );
        return f();
}

joque::task make_bmp_flash_task(
    std::filesystem::path const& firmware,
    std::filesystem::path const& device,
    joque::resource const&       dev_res )
{
        return {
            .job = joque::process::derive(
                       "arm-none-eabi-gdb",
                       "-nx",
                       "--batch",
                       "-ex",
                       "target extended-remote " + device.string() + "",
                       "-ex",
                       "monitor swdp_scan",
                       "-ex",
                       "attach 1",
                       "-ex",
                       "monitor erase_mass",
                       "-ex",
                       "load",
                       "-ex",
                       "compare-sections",
                       "-ex",
                       "kill",
                       firmware )
                       .add_input( firmware ),
            .resources = { dev_res },
        };
}

void flash_firmware_openocd(
    std::filesystem::path const& firmware,
    std::filesystem::path const& ocdconf )
{
        using namespace std::literals;

        std::stringstream ss;
        ss << "openocd -f " << ocdconf << " -c \"program " << firmware << " verify reset exit\" ";

        int const res = std::system( ss.str().data() );
        if ( res != 0 ) {
                EMLABCPP_INFO_LOG( "Executing: ", ss.str() );
                EMLABCPP_ERROR_LOG( "Flashing failed, error code: ", res );
                std::abort();
        }

        std::this_thread::sleep_for( 20ms );
}

}  // namespace servio::ftester
