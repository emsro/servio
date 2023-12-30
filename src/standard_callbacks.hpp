#include "callbacks.hpp"
#include "ctl/control.hpp"
#include "mtr/metrics.hpp"

#pragma once

namespace servio
{

struct standard_callbacks
{
        standard_callbacks(
            pwm_motor_interface&  motor,
            clk_interface&        clk,
            ctl::control&         ctl,
            mtr::metrics&         met,
            const cnv::converter& conv )
          : current_cb( motor, ctl, clk, conv )
          , pos_cb( ctl, met, clk, conv )
        {
        }

        void set_callbacks(
            period_interface&    period,
            period_cb_interface& period_cb,
            position_interface&  pos_drv,
            current_interface&   curr_drv )
        {
                period.set_period_callback( period_cb );
                curr_drv.set_current_callback( current_cb );
                pos_drv.set_position_callback( pos_cb );
        }

        current_callback  current_cb;
        position_callback pos_cb;
};

}  // namespace servio
