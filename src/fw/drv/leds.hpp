#include "base.hpp"
#include "drv_interfaces.hpp"
#include "fw/drv/defs.hpp"
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
        leds() = default;

        leds* setup(
            pin_cfg            red,
            pin_cfg            blue,
            TIM_HandleTypeDef& tim,
            pinch_cfg          yellow,
            pinch_cfg          green )
        {
                red_    = red;
                blue_   = blue;
                tim_    = &tim;
                yellow_ = yellow;
                green_  = green;
                return this;
        }

        em::result start();

        void force_red_led() override;

        void update( const leds_vals& leds ) override;

private:
        bool red_forced_ = false;

        pin_cfg red_;
        pin_cfg blue_;

        TIM_HandleTypeDef* tim_;

        pinch_cfg yellow_;
        pinch_cfg green_;
};

}  // namespace fw::drv
