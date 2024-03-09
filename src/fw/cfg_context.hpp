#pragma once

#include "brd/brd.hpp"
#include "cfg/map.hpp"
#include "cfg/storage.hpp"
#include "fw/load_persistent_config.hpp"

namespace servio::fw
{

struct cfg_context
{
        cfg::map                     map;
        cfg::payload                 last_payload;
        em::view< const brd::page* > pages;
};

inline cfg_context load_cfg_context()
{
        cfg_context res;

        res.map          = brd::get_default_config();
        res.pages        = brd::get_persistent_pages();
        res.last_payload = load_persistent_config( res.pages, res.map );

        return res;
}

}  // namespace servio::fw
