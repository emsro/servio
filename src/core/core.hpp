#pragma once

#include "callbacks.hpp"
#include "ctl/control.hpp"
#include "mon/indication.hpp"
#include "mon/monitor.hpp"
#include "mtr/metrics.hpp"

namespace servio::core
{

struct core
{

        ctl::control    ctl;
        cnv::converter  conv;
        mtr::metrics    met;
        mon::indication ind;
        mon::monitor    mon;

        core(
            base::microseconds     now,
            const drv::vcc_iface&  vcc_drv,
            const drv::temp_iface& temp_drv,
            drv::clk_iface&        clk )
          : ctl( now, ctl::config{} )
          , conv()
          , met( now, 0.F, { 0.F, 2 * base::pi } )
          , ind( now )
          , mon( now, ctl, vcc_drv, temp_drv, ind, conv )
        {
                ind.tick( clk.get_us() );
        }

        void tick( drv::leds_iface& leds, base::microseconds now )
        {
                mon.tick( now );
                ind.tick( now );
                leds.update( ind.get_state() );
        }
};

struct standard_callbacks
{
        standard_callbacks(
            drv::pwm_motor_iface& motor,
            drv::clk_iface&       clk,
            ctl::control&         ctl,
            mtr::metrics&         met,
            const cnv::converter& conv )
          : current_cb( motor, ctl, clk, conv )
          , pos_cb( ctl, met, clk, conv )
        {
        }

        void set_callbacks(
            drv::period_iface&    period,
            drv::period_cb_iface& period_cb,
            drv::pos_iface&       pos_drv,
            drv::curr_iface&      curr_drv )
        {
                period.set_period_callback( period_cb );
                curr_drv.set_current_callback( current_cb );
                pos_drv.set_position_callback( pos_cb );
        }

        current_callback  current_cb;
        position_callback pos_cb;
};

}  // namespace servio::core
