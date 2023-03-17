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
        converter  conv;
        metrics    met;
        indication ind;
        monitor    mon;

        core( microseconds now, acquisition& acqui, clock& clk )
          : ctl( now, ctl::config{} )
          , conv()
          , met( now, 0.f, { 0.f, 2 * pi } )
          , ind( now )
          , mon( now, ctl, acqui, ind, conv )
        {
                ind.tick( clk.get_us() );
        }

        void tick( leds& leds, microseconds now )
        {
                mon.tick( now );
                ind.tick( now );
                leds.update( ind.get_state() );
        }
};

struct standard_callbacks
{
        standard_callbacks(
            hbridge&         hb,
            acquisition&     acqui,
            clock&           clk,
            control&         ctl,
            metrics&         met,
            const converter& conv )
          : period_cb( acqui )
          , current_cb( hb, ctl, clk, conv )
          , pos_cb( ctl, met, clk, conv )
        {
                hb.set_period_callback( acquisition_period_callback{ acqui } );
                acqui.set_current_callback( current_cb );
                acqui.set_position_callback( pos_cb );
        }

        acquisition_period_callback period_cb;
        current_callback            current_cb;
        position_callback           pos_cb;
};

}  // namespace fw
