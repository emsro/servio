#include "control.hpp"
#include "fw/callbacks.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/drivers/hbridge.hpp"
#include "fw/drivers/leds.hpp"
#include "fw/monitor.hpp"
#include "fw/util.hpp"
#include "indication.hpp"
#include "metrics.hpp"

#pragma once

namespace fw
{

struct core
{

        control    ctl;
        metrics    met;
        indication ind;
        monitor    mon;

        core( std::chrono::milliseconds ms, acquisition& acqui )
          : ctl( ms, ctl::config{} )
          , met( ms, acqui.get_position(), { 0.f, 2 * pi } )
          , ind( ms )
          , mon( ms, ctl, acqui, ind )
        {
                ind.tick( fw::ticks_ms() );
        }

        void tick( leds& leds, std::chrono::milliseconds now )
        {
                mon.tick( now );
                ind.tick( now );
                leds.update( ind.get_state() );
        }
};

inline void setup_standard_callbacks( hbridge& hb, acquisition& acqui, control& ctl, metrics& met )
{

        hb.set_period_callback( acquisition_period_callback{ acqui, hb } );
        acqui.set_current_callback( current_callback{ hb, ctl } );
        acqui.set_position_callback( position_callback{ ctl, met } );
}

}  // namespace fw
