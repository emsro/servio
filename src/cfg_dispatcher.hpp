#include "cfg/key.hpp"
#include "cfg/map.hpp"
#include "core.hpp"

#pragma once

struct cfg_dispatcher
{

        cfg::map& map;
        core&     c;

        template < cfg::key Key, typename T >
        void set( const T& item )
        {
                map.set_val< Key >( item );
                apply( Key );
        }

        void full_apply();

        void apply( const cfg::key& key );
};
