
#include "cfg/key.hpp"
#include "cfg/map.hpp"
#include "core/core.hpp"

#pragma once

namespace servio::cfg
{

struct dispatcher
{

        map&        m;
        core::core& c;

        template < key Key, typename T >
        void set( const T& item )
        {
                m.set_val< Key >( item );
                apply( Key );
        }

        void full_apply();

        void apply( const key& key );

        template < key Key >
        const auto& get() const
        {
                return m.get_val< Key >();
        }
};

void apply( ctl::control&, const map&, const key& key );

}  // namespace servio::cfg
