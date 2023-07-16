#include "control.hpp"
#include "fw/callbacks.hpp"
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

        core(
            microseconds                      now,
            const drv::vcc_interface&         vcc_drv,
            const drv::temperature_interface& temp_drv,
            drv::clock&                       clk )
          : ctl( now, ctl::config{} )
          , conv()
          , met( now, 0.f, { 0.f, 2 * pi } )
          , ind( now )
          , mon( now, ctl, vcc_drv, temp_drv, ind, conv )
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
            drv::hbridge&             hb,
            drv::clock&               clk,
            drv::position_interface&  pos_drv,
            drv::current_interface&   curr_drv,
            drv::period_cb_interface& period_cb,
            control&                  ctl,
            metrics&                  met,
            const converter&          conv )
          : current_cb( hb, ctl, clk, conv )
          , pos_cb( ctl, met, clk, conv )
        {
                hb.set_period_callback( period_cb );
                curr_drv.set_current_callback( current_cb );
                pos_drv.set_position_callback( pos_cb );
        }

        current_callback  current_cb;
        position_callback pos_cb;
};

}  // namespace fw
