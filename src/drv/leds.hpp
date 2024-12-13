#include "../base/base.hpp"
#include "../plt/platform.hpp"
#include "./defs.hpp"
#include "./interfaces.hpp"

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

        leds* setup( pin_cfg red, pin_cfg blue, uint32_t y_chan, uint32_t g_chan )
        {
                red_    = red;
                blue_   = blue;
                y_chan_ = y_chan;
                g_chan_ = g_chan;
                return tim_ != nullptr ? this : nullptr;
        }

        em::result start();

        void force_red_led() override;

        void update( leds_vals const& leds ) override;

private:
        bool red_forced_ = false;

        pin_cfg red_;
        pin_cfg blue_;

        TIM_HandleTypeDef* tim_;

        uint32_t y_chan_;
        uint32_t g_chan_;
};

}  // namespace servio::drv
