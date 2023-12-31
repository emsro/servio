#include "base/drv_interfaces.hpp"
#include "core/globals.hpp"

#pragma once

namespace servio::fw
{
inline void install_stop_callback( base::leds_interface* leds_ptr )
{
        if ( leds_ptr != nullptr ) {
                core::STOP_CALLBACK = [leds_ptr] {
                        leds_ptr->force_red_led();
                };
        }
};
}  // namespace servio::fw
