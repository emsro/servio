#include "base.hpp"
#include "drv_interfaces.hpp"
#include "platform.hpp"

#include <cstdint>
#include <emlabcpp/experimental/function_view.h>

#pragma once

namespace em = emlabcpp;

namespace fw::drv
{

class leds : public leds_interface
{
public:
        struct handles
        {
                GPIO_TypeDef* red_peripheral;
                uint16_t      red_pin;

                GPIO_TypeDef* blue_peripheral;
                uint16_t      blue_pin;

                TIM_HandleTypeDef tim;

                uint32_t yellow_channel;
                uint32_t green_channel;
        };

        leds() = default;

        leds* setup( auto&& setup_f )
        {
                if ( setup_f( h_ ) != em::SUCCESS ) {
                        return nullptr;
                }
                return this;
        }

        em::result start();

        void force_red_led();

        void update( const leds_vals& leds );

private:
        bool    red_forced_ = false;
        handles h_;
};

}  // namespace fw::drv
