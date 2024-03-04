#include "base/drv_interfaces.hpp"

#pragma once

namespace servio::fw
{
inline void install_stop_callback(
    auto&                      target,
    base::pwm_motor_interface& motor,
    base::leds_interface*      leds_ptr )
{
        target = [&motor, leds_ptr] {
                motor.force_stop();
                if ( leds_ptr != nullptr )
                        leds_ptr->force_red_led();
        };
};
}  // namespace servio::fw
