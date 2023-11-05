#include "drv_interfaces.hpp"
#include "globals.hpp"

#pragma once

namespace fw
{
inline void install_stop_callback( leds_interface* leds_ptr )
{
        if ( leds_ptr != nullptr ) {
                STOP_CALLBACK = [leds_ptr] {
                        leds_ptr->force_red_led();
                };
        }
};
}  // namespace fw
