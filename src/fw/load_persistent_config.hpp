#include "cfg/storage.hpp"
#include "fw/util.hpp"

#pragma once

namespace servio::fw
{

using page = em::view< std::byte* >;

inline cfg::payload load_persistent_config( em::view< const page* > pages, cfg::map& cfg )
{
        const cfg::page* last_page = cfg::find_latest_page( pages );

        cfg::payload res{ .git_ver = "", .git_date = "", .id = 0 };
        if ( last_page != nullptr ) {
                auto check_f = [&]( const cfg::payload& payload ) {
                        res = payload;
                        return true;
                };
                const bool cfg_loaded = cfg::load( *last_page, check_f, cfg );

                if ( !cfg_loaded ) {
                        fw::stop_exec();
                }
        }

        return res;
}

}  // namespace servio::fw
