#include "cnv/converter.hpp"
#include "cnv/utils.hpp"
#include "ctl/control.hpp"
#include "mtr/metrics.hpp"

#pragma once

namespace servio::core
{

class current_callback : public drv::current_cb_iface
{
public:
        current_callback(
            drv::pwm_motor_iface& motor,
            ctl::control&         ctl,
            drv::clk_iface&       clk,
            const cnv::converter& conv )
          : motor_( motor )
          , ctl_( ctl )
          , clk_( clk )
          , conv_( conv )
        {
        }

        [[gnu::flatten]] void on_value_irq( uint32_t curr, std::span< uint16_t > ) override
        {
                const float c = cnv::current( conv_, curr, motor_ );

                ctl_.current_irq( clk_.get_us(), c );
                motor_.set_power( ctl_.get_power() );
        }

private:
        drv::pwm_motor_iface& motor_;
        ctl::control&         ctl_;
        drv::clk_iface&       clk_;
        const cnv::converter& conv_;
};

class position_callback : public drv::position_cb_iface
{
public:
        position_callback(
            ctl::control&         ctl,
            mtr::metrics&         met,
            drv::clk_iface&       clk,
            const cnv::converter& conv )
          : ctl_( ctl )
          , met_( met )
          , clk_( clk )
          , conv_( conv )
        {
        }

        [[gnu::flatten]] void on_value_irq( uint32_t position ) override
        {
                const base::microseconds now = clk_.get_us();

                const float p = conv_.position.convert( position );

                met_.position_irq( now, p );
                ctl_.moving_irq( now, met_.is_moving() );
                ctl_.position_irq( now, met_.get_position() );
                ctl_.velocity_irq( now, met_.get_velocity() );
        }

private:
        ctl::control&         ctl_;
        mtr::metrics&         met_;
        drv::clk_iface&       clk_;
        const cnv::converter& conv_;
};

}  // namespace servio::core
