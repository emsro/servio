
#include "cfg/key.hpp"
#include "cfg/map.hpp"
#include "core.hpp"

#pragma once

namespace cfg
{

struct dispatcher
{

        map&  m;
        core& c;

        template < key Key, typename T >
        void set( const T& item )
        {
                m.set_val< Key >( item );
                apply( Key );
        }

        void full_apply();

        void apply( const key& key );
};

void apply( control&, const map&, const key& key );

}  // namespace cfg