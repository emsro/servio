#include "control.hpp"
#include "fw/callbacks.hpp"
#include "fw/drv/acquisition.hpp"
#include "fw/drv/hbridge.hpp"
#include "fw/drv/leds.hpp"
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

        core( microseconds now, drv::acquisition& acqui, drv::clock& clk )
          : ctl( now, ctl::config{} )
          , conv()
          , met( now, 0.f, { 0.f, 2 * pi } )
          , ind( now )
          , mon( now, ctl, acqui, ind, conv )
        {
                ind.tick( clk.get_us() );
        }

        void tick( drv::leds& leds, microseconds now )
        {
                mon.tick( now );
                ind.tick( now );
                leds.update( ind.get_state() );
        }
};

struct standard_callbacks
{
        standard_callbacks(
            drv::hbridge&     hb,
            drv::acquisition& acqui,
            drv::clock&       clk,
            control&          ctl,
            metrics&          met,
            const converter&  conv )
          : period_cb( acqui )
          , current_cb( hb, ctl, clk, conv )
          , pos_cb( ctl, met, clk, conv )
        {
                hb.set_period_callback( period_cb );
                acqui.set_current_callback( current_cb );
                acqui.set_position_callback( pos_cb );
        }

        acquisition_period_callback period_cb;
        current_callback            current_cb;
        position_callback           pos_cb;
};

}  // namespace fw
