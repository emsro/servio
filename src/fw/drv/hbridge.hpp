#include "drv_interfaces.hpp"
#include "fw/drv/callbacks.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <span>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;

namespace fw::drv
{

inline empty_period_cb EMPTY_PERIOD_CB;

class hbridge : public pwm_motor_interface, public period_interface
{
public:
        // HAL handles used by the driver
        struct handles
        {
                TIM_HandleTypeDef timer;
                uint32_t          mc1_channel;
                uint32_t          mc2_channel;
        };

        hbridge()
          : period_cb_( &EMPTY_PERIOD_CB )
        {
        }

        // driver is non-movable and non-copyable
        hbridge( const hbridge& )            = delete;
        hbridge( hbridge&& )                 = delete;
        hbridge& operator=( const hbridge& ) = delete;
        hbridge& operator=( hbridge&& )      = delete;

        hbridge* setup( auto&& setup_f )
        {

                if ( setup_f( h_ ) != em::SUCCESS ) {
                        return nullptr;
                }

                timer_max_ = h_.timer.Init.Period;

                set_power( 0 );

                return this;
        }

        void timer_period_irq( TIM_HandleTypeDef* h )
        {
                if ( h != &h_.timer ) {
                        return;
                }
                period_cb_->on_period_irq();
        }

        void timer_irq()
        {
                HAL_TIM_IRQHandler( &h_.timer );
        }

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

        status get_status() const override
        {
                return status::NOMINAL;
        };

private:
        period_cb_interface* period_cb_;
        uint32_t             timer_max_ = 0;
        handles              h_{};
};

}  // namespace fw::drv
