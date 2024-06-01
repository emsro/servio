#include "cfg/storage.hpp"
#include "drv/interfaces.hpp"

#pragma once

namespace servio::fw
{

struct persistent_config_writer
{
        cfg::payload&       pl;
        drv::storage_iface& iface;

        bool operator()( const cfg::map* cfg );
};

}  // namespace servio::fw
