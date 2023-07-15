#include "config.hpp"
#include "fw/drv/acquisition.hpp"
#include "fw/drv/clock.hpp"
#include "fw/drv/comms.hpp"
#include "fw/drv/debug_comms.hpp"
#include "fw/drv/hbridge.hpp"
#include "fw/drv/leds.hpp"
#include "globals.hpp"

#pragma once

namespace brd
{

em::view< const em::view< std::byte* >* > get_persistent_pages();
cfg_map                                   get_config();

void                  setup_board();
fw::drv::clock*       setup_clock();
fw::drv::acquisition* setup_acquisition();
fw::drv::hbridge*     setup_hbridge();
fw::drv::comms*       setup_comms();
fw::drv::debug_comms* setup_debug_comms();
fw::drv::leds*        setup_leds();

}  // namespace brd

namespace fw
{
inline drv::leds* setup_leds_with_stop_callback()
{
        drv::leds* leds_ptr = brd::setup_leds();
        if ( leds_ptr != nullptr ) {
                STOP_CALLBACK = [leds_ptr] {
                        leds_ptr->force_red_led();
                };
        }
        return leds_ptr;
};
}  // namespace fw
