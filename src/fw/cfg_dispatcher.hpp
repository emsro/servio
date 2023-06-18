#include "config.hpp"
#include "fw/core.hpp"

#pragma once

namespace fw
{

struct cfg_dispatcher
{

        cfg_map& map;
        core&    c;

        cfg_value_message get( const cfg_key& key );

        template < cfg_key key, typename T >
        void set( const T& item )
        {
                map.set_val< key >( item );
                apply( key );
        }

        void full_apply();

        void apply( const cfg_key& key );
};

}  // namespace fw
