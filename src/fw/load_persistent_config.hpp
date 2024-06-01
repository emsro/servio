#include "cfg/storage.hpp"
#include "drv/interfaces.hpp"
#include "fw/util.hpp"

#pragma once

namespace servio::fw
{

using page = em::view< std::byte* >;

inline cfg::payload load_persistent_config( drv::storage_iface& iface, cfg::map& cfg )
{
        constexpr std::size_t buffer_n = cfg::map::registers_count * sizeof( cfg::keyval ) + 128;
        std::array< std::byte, buffer_n > buffer;
        em::outcome                       r = iface.load_page( buffer );
        if ( r == em::ERROR )
                fw::stop_exec();  // XXX: this is drastic

        cfg::payload res{ .git_ver = "", .git_date = "", .id = 0 };
        if ( r == em::FAILURE )
                return res;

        auto check_f = [&]( const cfg::payload& payload ) {
                res = payload;
                return true;
        };
        const bool cfg_loaded = cfg::load( buffer, check_f, cfg );

        if ( !cfg_loaded )
                fw::stop_exec();

        return res;
}

}  // namespace servio::fw
