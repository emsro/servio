
#pragma once

#include "../core/core.hpp"
#include "../core/drivers.hpp"
#include "./def.hpp"

namespace servio::cfg
{

struct dispatcher
{

        map&                  m;
        ctl::control&         ctl;
        cnv::converter&       conv;
        mtr::metrics&         met;
        mon::monitor&         mon;
        drv::pwm_motor_iface& motor;
        drv::get_pos_iface&   pos;

        template < key Key, typename T >
        void set( T const& item )
        {
                m.ref_by_key< Key >() = item;
                this->apply( Key );
        }

        template < key Key >
        auto const& get() const
        {
                return m.ref_by_key< Key >();
        }

        void full_apply();
        void apply( key k );
};

}  // namespace servio::cfg
