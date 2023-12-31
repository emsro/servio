#include "cnv/converter.hpp"
#include "cnv/utils.hpp"
#include "ctl/control.hpp"
#include "mtr/metrics.hpp"

#pragma once

namespace servio
{

class current_callback : public base::current_cb_interface
{
public:
        current_callback(
            base::pwm_motor_interface& motor,
            ctl::control&              ctl,
            base::clk_interface&       clk,
            const cnv::converter&      conv )
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
        base::pwm_motor_interface& motor_;
        ctl::control&              ctl_;
        base::clk_interface&       clk_;
        const cnv::converter&      conv_;
};

class position_callback : public base::position_cb_interface
{
public:
        position_callback(
            ctl::control&         ctl,
            mtr::metrics&         met,
            base::clk_interface&  clk,
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
        base::clk_interface&  clk_;
        const cnv::converter& conv_;
};

}  // namespace servio
