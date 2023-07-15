#include "control.hpp"
#include "converter.hpp"
#include "drv/acquisition.hpp"
#include "drv/clock.hpp"
#include "drv/hbridge.hpp"
#include "fw/conversion.hpp"
#include "metrics.hpp"
#include "stm32g4xx_hal.h"

#pragma once

namespace fw
{

class acquisition_period_callback : public drv::period_cb_interface
{
public:
        acquisition_period_callback( drv::acquisition& acq )
          : acq_( acq )
        {
        }

        virtual void on_period()
        {
                acq_.period_elapsed_irq();
        }

private:
        drv::acquisition& acq_;
};

class current_callback : public drv::current_cb_interface
{
public:
        current_callback( drv::hbridge& hb, control& ctl, drv::clock& clk, const converter& conv )
          : hb_( hb )
          , ctl_( ctl )
          , clk_( clk )
          , conv_( conv )
        {
        }

        virtual void on_current( uint32_t curr, std::span< uint16_t > )
        {
                float c = current( conv_, curr, hb_ );

                ctl_.current_irq( clk_.get_us(), c );
                hb_.set_power( ctl_.get_power() );
        }

private:
        drv::hbridge&    hb_;
        control&         ctl_;
        drv::clock&      clk_;
        const converter& conv_;
};

class position_callback : public drv::position_cb_interface
{
public:
        position_callback( control& ctl, metrics& met, drv::clock& clk, const converter& conv )
          : ctl_( ctl )
          , met_( met )
          , clk_( clk )
          , conv_( conv )
        {
        }

        virtual void on_position( uint32_t position )
        {
                microseconds now = clk_.get_us();

                float p = conv_.position.convert( position );

                met_.position_irq( now, p );
                ctl_.moving_irq( now, met_.is_moving() );
                ctl_.position_irq( now, met_.get_position() );
                ctl_.velocity_irq( now, met_.get_velocity() );
        }

private:
        control&         ctl_;
        metrics&         met_;
        drv::clock&      clk_;
        const converter& conv_;
};

}  // namespace fw
