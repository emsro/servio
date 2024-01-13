#include "store_persistent_config.hpp"

#include "git.h"
#include "platform.hpp"
#include "util.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/defer.h>


namespace servio::fw
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
        auto             start_addr = std::bit_cast< uint32_t >( start );

        plt::cfg_erase( start_addr );

        constexpr std::size_t buffer_n = cfg::map::registers_count * sizeof( cfg::keyval ) + 128;

        std::array< std::byte, buffer_n > buffer;

        auto [succ, used_buffer] = cfg::store( pld, cfg, buffer );

        plt::cfg_write( start_addr, used_buffer );

        return succ;
}

bool persistent_config_writer::operator()( const cfg::map* cfg )
{
        const cfg::payload pld{
            .git_ver  = git::Describe(),
            .git_date = git::CommitDate(),
            .id       = last_payload.id + 1,
        };
        const cfg::page* page = cfg::find_next_page( pages );

        if ( page == nullptr ) {
                return false;
        }
        const bool succ = fw::store_persistent_config( *page, pld, cfg );
        if ( succ ) {
                last_payload = pld;
        }
        // TODO: why is this happening?
        if ( current_iface.get_status() == base::status::DEGRADED ) {
                current_iface.clear_status();
        }
        return succ;
}

}  // namespace servio::fw
