#pragma once

#include "ctl/control.hpp"
#include "indication.hpp"
#include "metrics.hpp"
#include "monitor.hpp"

namespace servio
{

struct core
{

        ctl::control ctl;
        converter    conv;
        metrics      met;
        indication   ind;
        monitor      mon;

        core(
            microseconds                 now,
            const vcc_interface&         vcc_drv,
            const temperature_interface& temp_drv,
            clk_interface&               clk )
          : ctl( now, ctl::config{} )
          , conv()
          , met( now, 0.F, { 0.F, 2 * pi } )
          , ind( now )
          , mon( now, ctl, vcc_drv, temp_drv, ind, conv )
        {
                ind.tick( clk.get_us() );
        }

        void tick( leds_interface& leds, microseconds now )
        {
                mon.tick( now );
                ind.tick( now );
                leds.update( ind.get_state() );
        }
};

}  // namespace servio
