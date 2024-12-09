
#include "../core/core.hpp"
#include "../core/drivers.hpp"
#include "./key.hpp"
#include "./map.hpp"

#pragma once

namespace servio::cfg
{

struct dispatcher
{

        map&                  m;
        core::core&           c;
        drv::pwm_motor_iface& motor;
        drv::pos_iface&       pos;

        template < key Key, typename T >
        void set( T const& item )
        {
                m.set_val< Key >( item );
                apply( Key );
        }

        void full_apply();

        void apply( key const& key );

        template < key Key >
        auto const& get() const
        {
                return m.get_val< Key >();
        }
};

void apply( ctl::control&, map const&, key const& key );

}  // namespace servio::cfg
