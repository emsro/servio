#include "drv/callbacks.hpp"
#include "drv/interfaces.hpp"
#include "platform.hpp"
#include "sntr/sentry.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <span>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

inline empty_period_cb EMPTY_PERIOD_CB;

class hbridge : public pwm_motor_interface, public period_interface
{
public:
        hbridge( TIM_HandleTypeDef* tim );

        // driver is non-movable and non-copyable
        hbridge( const hbridge& )            = delete;
        hbridge( hbridge&& )                 = delete;
        hbridge& operator=( const hbridge& ) = delete;
        hbridge& operator=( hbridge&& )      = delete;

        hbridge* setup( uint32_t mc1_channel, uint32_t mc2_channel );

        void force_stop() override;

        void timer_period_irq( TIM_HandleTypeDef* h );

        void                 set_period_callback( period_cb_interface& ) override;
        period_cb_interface& get_period_callback() override;

        // Sets the power that hbridge should generate to the motor.
        // Input range is lineary interpolated based on:
        //  - limits<int16_t>::lower is full reverse
        //  - 0 is nothing
        //  - limits<int16_t>::max is full forward
        void set_power( int16_t ) override;

        // returns either -1 or 1 based on direction last set to power
        int8_t get_direction() const override;

        // starts the timers
        em::result start() override;
        em::result stop() override;

private:
        period_cb_interface* period_cb_;
        uint32_t             timer_max_ = 0;
        TIM_HandleTypeDef*   tim_;
        uint32_t             mc1_channel_ = 0;
        uint32_t             mc2_channel_ = 0;
};

inline hbridge::hbridge( TIM_HandleTypeDef* tim )
  : period_cb_( &EMPTY_PERIOD_CB )
  , tim_( tim )
{
}

inline hbridge* hbridge::setup( uint32_t mc1_channel, uint32_t mc2_channel )
{
        mc1_channel_ = mc1_channel;
        mc2_channel_ = mc2_channel;

        if ( tim_ != nullptr )
                timer_max_ = tim_->Init.Period;
        else
                return nullptr;

        set_power( 0 );

        return this;
}

inline void hbridge::timer_period_irq( TIM_HandleTypeDef* h )
{
        if ( h != tim_ )
                return;
        period_cb_->on_period_irq();
}

}  // namespace servio::drv
