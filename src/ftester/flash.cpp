#include "ftester/flash.hpp"

#include <emlabcpp/experimental/logging.h>
#include <thread>

namespace servio::ftester
{

void flash_firmware( const std::filesystem::path& firmware, const std::filesystem::path& ocdconf )
{
        using namespace std::literals;

        std::stringstream ss;
        ss << "openocd -f " << ocdconf << " -c \"program " << firmware << " verify reset exit\" ";
        EMLABCPP_DEBUG_LOG( "Executing: ", ss.str() );

        const int res = std::system( ss.str().data() );
        if ( res != 0 ) {
                EMLABCPP_ERROR_LOG( "Flashing failed, error code: ", res );
                std::abort();
        }

        std::this_thread::sleep_for( 20ms );
}

}  // namespace servio::ftester
