#include "config.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/drivers/comms.hpp"
#include "fw/drivers/debug_comms.hpp"
#include "fw/drivers/hbridge.hpp"
#include "fw/drivers/leds.hpp"

#pragma once

namespace brd
{

void             apply_config( em::function_view< void( const cfg_keyval& ) > f );
void             setup_board();
fw::acquisition* setup_acquisition();
fw::hbridge*     setup_hbridge();
fw::comms*       setup_comms();
fw::debug_comms* setup_debug_comms();
fw::leds*        setup_leds();

}  // namespace brd
