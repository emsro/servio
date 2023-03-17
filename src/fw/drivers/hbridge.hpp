#include "config.hpp"
#include "fw/util.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_function.h>
#include <span>

#pragma once

namespace fw
{

class hbridge
{
public:
        // HAL handles used by the driver
        struct handles
        {
                TIM_HandleTypeDef timer;
                uint32_t          mc1_channel;
                uint32_t          mc2_channel;
        };

        using period_callback = em::static_function< void(), 16 >;

        hbridge() = default;

        // driver is non-movable and non-copyable
        hbridge( const hbridge& )            = delete;
        hbridge( hbridge&& )                 = delete;
        hbridge& operator=( const hbridge& ) = delete;
        hbridge& operator=( hbridge&& )      = delete;

        bool setup( em::function_view< bool( handles& ) > );

        void timer_period_irq();
        void timer_irq();

        void                   set_period_callback( period_callback );
        const period_callback& get_period_callback();

        // Sets the power that hbridge should generate to the motor.
        // Input range is lineary interpolated based on:
        //  - limits<int16_t>::lower is full reverse
        //  - 0 is nothing
        //  - limits<int16_t>::max is full forward
        void set_power( int16_t );

        // returns either -1 or 1 based on direction last set to power
        int8_t get_direction() const;

        // starts the timers
        void start();
        
        void stop();

private:
        period_callback period_cb_;
        uint32_t        timer_max_ = 0;
        handles         h_{};
};

}  // namespace fw
