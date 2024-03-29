#include "cfg/storage.hpp"
#include "drv/interfaces.hpp"

#pragma once

namespace servio::fw
{

bool store_persistent_config( const cfg::page& page, const cfg::payload& pld, const cfg::map* cfg );

struct persistent_config_writer
{
        cfg::payload&                last_payload;
        em::view< const cfg::page* > pages;

        bool operator()( const cfg::map* cfg );
};

}  // namespace servio::fw
