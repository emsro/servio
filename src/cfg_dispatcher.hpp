#include "config.hpp"
#include "core.hpp"

#pragma once

struct cfg_dispatcher
{

        cfg_map& map;
        core&    c;

        template < cfg_key Key, typename T >
        void set( const T& item )
        {
                map.set_val< Key >( item );
                apply( Key );
        }

        void full_apply();

        void apply( const cfg_key& key );
};
