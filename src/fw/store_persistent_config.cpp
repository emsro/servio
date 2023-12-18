#include "store_persistent_config.hpp"

#include "platform.hpp"
#include "util.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/defer.h>


namespace fw
{

bool store_persistent_config( const cfg::page& page, const cfg::payload& pld, const cfg::map* cfg )
{

        if ( HAL_FLASH_Unlock() != HAL_OK ) {
                stop_exec();
        }
        const em::defer d = [] {
                if ( HAL_FLASH_Lock() != HAL_OK ) {
                        stop_exec();
                }
        };

        const std::byte* start      = page.begin();
        auto             start_addr = reinterpret_cast< uint32_t >( start );

        plt::cfg_erase( start_addr );

        auto f = [&]( std::size_t offset, uint64_t val ) -> bool {
                return plt::cfg_write( start_addr + offset, val );
        };

        const bool succ = cfg::store( pld, cfg, f );

        return succ;
}

}  // namespace fw
