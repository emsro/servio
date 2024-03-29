#include "base/base.hpp"
#include "drv/defs.hpp"
#include "drv/interfaces.hpp"
#include "platform.hpp"

#include <cstdint>
#include <emlabcpp/experimental/function_view.h>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

class leds : public leds_iface
{
public:
        leds( TIM_HandleTypeDef* tim )
          : tim_( tim )
        {
        }

        leds* setup( pin_cfg red, pin_cfg blue, pinch_cfg yellow, pinch_cfg green )
        {
                red_    = red;
                blue_   = blue;
                yellow_ = yellow;
                green_  = green;
                return tim_ != nullptr ? this : nullptr;
        }

        em::result start();

        void force_red_led() override;

        void update( const base::leds_vals& leds ) override;

private:
        bool red_forced_ = false;

        pin_cfg red_;
        pin_cfg blue_;

        TIM_HandleTypeDef* tim_;

        pinch_cfg yellow_;
        pinch_cfg green_;
};

}  // namespace servio::drv
