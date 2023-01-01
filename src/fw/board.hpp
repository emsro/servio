#include "config.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/drivers/comms.hpp"
#include "fw/drivers/debug_comms.hpp"
#include "fw/drivers/hbridge.hpp"

#pragma once

namespace fw
{

void         apply_config( em::function_view< void( const cfg_keyval& ) > f );
void         setup_board();
acquisition* setup_acquisition();
hbridge*     setup_hbridge();
comms*       setup_comms();
debug_comms* setup_debug_comms();

}  // namespace fw
