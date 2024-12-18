#include "../cnv/converter.hpp"
#include "../cnv/utils.hpp"
#include "../ctl/control.hpp"
#include "../mtr/metrics.hpp"

#pragma once

namespace servio::core
{

struct avg_filter
{
        static constexpr std::size_t n      = 16;
        std::array< float, n >       buffer = { 0 };
        std::size_t                  i      = 0;

        void add( float v )
        {
                buffer[i] = v;
                i         = ( i + 1 ) % n;
        }

        float get() const
        {
                return em::avg( buffer );
        }
};

class current_callback : public drv::current_cb_iface
{
public:
        current_callback(
            drv::pwm_motor_iface& motor,
            ctl::control&         ctl,
            drv::clk_iface&       clk,
            cnv::converter const& conv )
          : motor_( motor )
          , ctl_( ctl )
          , clk_( clk )
          , conv_( conv )
        {
        }

        [[gnu::flatten]] void on_value_irq( uint32_t curr, std::span< uint16_t > ) override
        {
                float const c = cnv::current( conv_, curr, motor_ );

                filter_.add( c );

                ctl_.current_irq( clk_.get_us(), filter_.get() );
                motor_.set_power( ctl_.get_power() );
        }

private:
        avg_filter            filter_;
        drv::pwm_motor_iface& motor_;
        ctl::control&         ctl_;
        drv::clk_iface&       clk_;
        cnv::converter const& conv_;
};

class position_callback : public drv::position_cb_iface
{
public:
        position_callback(
            ctl::control&         ctl,
            mtr::metrics&         met,
            drv::clk_iface&       clk,
            cnv::converter const& conv )
          : ctl_( ctl )
          , met_( met )
          , clk_( clk )
          , conv_( conv )
        {
        }

        [[gnu::flatten]] void on_value_irq( uint32_t position ) override
        {
                microseconds const now = clk_.get_us();

                float const p = conv_.position.convert( position );

                met_.position_irq( now, p );
                ctl_.moving_irq( now, met_.is_moving() );
                ctl_.position_irq( now, met_.get_position() );
                ctl_.velocity_irq( now, met_.get_velocity() );
        }

private:
        ctl::control&         ctl_;
        mtr::metrics&         met_;
        drv::clk_iface&       clk_;
        cnv::converter const& conv_;
};

}  // namespace servio::core
