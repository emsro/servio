#include "drv/interfaces.hpp"

#pragma once

namespace servio::fw
{
inline void
install_stop_callback( auto& target, drv::pwm_motor_iface& motor, drv::leds_iface* leds_ptr )
{
        target = [&motor, leds_ptr] {
                motor.force_stop();
                if ( leds_ptr != nullptr )
                        leds_ptr->force_red_led();
        };
};
}  // namespace servio::fw
