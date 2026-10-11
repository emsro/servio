#pragma once

#include "../gov/governor_manager.hpp"
#include "../mon/indication.hpp"
#include "../mon/monitor.hpp"
#include "../mtr/metrics.hpp"

namespace servio::core
{

struct core
{

        gov::governors_ctx& govs;
        cnv::converter      conv;
        mtr::metrics        met;
        mon::indication     ind;
        mon::monitor        mon;

        core(
            gov::governors_ctx&    govs,
            microseconds           now,
            drv::vcc_iface const&  vcc_drv,
            drv::temp_iface const& temp_drv )
          : govs( govs )
          , conv()
          , met( now, 0.F, 0.F, { 0.F, 2 * pi } )
          , ind()
          , mon( now, vcc_drv, temp_drv, ind, conv )
        {
        }

        void tick( drv::leds_iface& leds, microseconds now )
        {
                mon.tick( now, !!govs.manager.active() );
                ind.tick( now );
                leds.update( ind.get_state() );
        }
};

}  // namespace servio::core
