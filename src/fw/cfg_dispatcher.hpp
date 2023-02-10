#include "config.hpp"
#include "control.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/monitor.hpp"

#pragma once

namespace fw
{

struct cfg_dispatcher
{

    cfg_map&     map;
    acquisition& acq;
    control&     ctl;
    monitor&     m;

    void operator()( const cfg_keyval& kv )
    {
        set( kv.key, kv.msg );
    }

    cfg_value_message get( const cfg_key& key );

    void set( const cfg_key& key, const cfg_value_message& msg );

    void full_apply();

    void apply( const cfg_key& key );
};

}  // namespace fw
