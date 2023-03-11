#include "config.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/drivers/clock.hpp"
#include "fw/drivers/comms.hpp"
#include "fw/drivers/debug_comms.hpp"
#include "fw/drivers/hbridge.hpp"
#include "fw/drivers/leds.hpp"
#include "globals.hpp"

#pragma once

namespace brd
{

void             apply_config( em::function_view< void( const cfg_keyval& ) > f );
void             setup_board();
fw::clock*       setup_clock();
fw::acquisition* setup_acquisition();
fw::hbridge*     setup_hbridge();
fw::comms*       setup_comms();
fw::debug_comms* setup_debug_comms();
fw::leds*        setup_leds();

}  // namespace brd

namespace fw
{
inline leds* setup_leds_with_stop_callback()
{
        leds* leds_ptr = brd::setup_leds();
        if ( leds_ptr != nullptr ) {
                STOP_CALLBACK = [leds_ptr] {
                        leds_ptr->force_red_led();
                };
        }
        return leds_ptr;
};
}  // namespace fw
