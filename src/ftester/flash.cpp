#include "./flash.hpp"

#include <emlabcpp/experimental/logging.h>
#include <joque/process.hpp>
#include <thread>

namespace servio::ftester
{

joque::task make_flash_task(
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

void flash_firmware( std::filesystem::path const& firmware, std::filesystem::path const& ocdconf )
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
