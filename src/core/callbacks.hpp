#include "../cnv/converter.hpp"
#include "../cnv/utils.hpp"
#include "../gov/governor_manager.hpp"
#include "../mtr/metrics.hpp"

#pragma once

namespace servio::core
{

class current_callback : public drv::current_cb_iface
{
public:
        current_callback(
            drv::pwm_motor_iface&  motor,
            gov::governor_manager& gov,
            drv::clk_iface&        clk,
            cnv::converter const&  conv )
          : motor_( motor )
          , gov_( gov )
          , clk_( clk )
          , conv_( conv )
        {
        }

        [[gnu::flatten]] void on_value_irq( uint32_t curr, std::span< uint16_t > ) override
        {
                float const c = cnv::current( conv_, curr, motor_ );

                auto pow = gov_.current_irq( clk_.get_us(), c );
                motor_.set_power( pow );
        }

private:
        drv::pwm_motor_iface&  motor_;
        gov::governor_manager& gov_;
        drv::clk_iface&        clk_;
        cnv::converter const&  conv_;
};

class position_callback : public drv::position_cb_iface
{
public:
        position_callback(
            gov::governor_manager& gov,
            mtr::metrics&          met,
            drv::clk_iface&        clk,
            cnv::converter const&  conv )
          : gov_( gov )
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
                gov_.metrics_irq( now, p, met_.get_velocity(), met_.is_moving() );
        }

private:
        gov::governor_manager& gov_;
        mtr::metrics&          met_;
        drv::clk_iface&        clk_;
        cnv::converter const&  conv_;
};

struct standard_callbacks
{
        standard_callbacks(
            drv::pwm_motor_iface&  motor,
            drv::clk_iface&        clk,
            gov::governor_manager& gov,
            mtr::metrics&          met,
            cnv::converter const&  conv )
          : current_cb( motor, gov, clk, conv )
          , pos_cb( gov, met, clk, conv )
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
