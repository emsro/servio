#include "../base.hpp"
#include "../plt/platform.hpp"
#include "./defs.hpp"
#include "./interfaces.hpp"

#include <cstdint>
#include <emlabcpp/experimental/function_view.h>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

struct leds : public leds_iface
{

        leds* setup( pin_cfg red, pin_cfg blue, pin_cfg green )
        {
                red_   = red;
                blue_  = blue;
                green_ = green;
                return tim_ != nullptr ? this : nullptr;
        }

        void force_red_led() override;

        void update( leds_vals const& leds ) override;

private:
        bool red_forced_ = false;

        pin_cfg red_;
        pin_cfg blue_;
        pin_cfg green_;

        TIM_HandleTypeDef* tim_;
};

}  // namespace servio::drv
