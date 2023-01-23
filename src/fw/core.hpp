#include "control.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/monitor.hpp"
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
      : ctl( ms )
      , met( ms, acqui.get_position() )
      , ind( ms )
      , mon( ms, ctl, acqui, ind )
    {
        ind.tick( fw::ticks_ms() );
    }
};

void setup_standard_callbacks( hbridge& hb, acquisition& acqui, control& ctl, metrics& met )
{

    hb.set_period_callback( acquisition_period_callback{ acqui, hb } );
    acqui.set_current_callback( current_callback{ hb, ctl } );
    acqui.set_position_callback( position_callback{ ctl, met } );
}

}  // namespace fw
