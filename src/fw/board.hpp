#include "config.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/drivers/comms.hpp"
#include "fw/drivers/debug_comms.hpp"
#include "fw/drivers/hbridge.hpp"

#pragma once

namespace fw
{

void         setup_board();
void         setup_config( config& );
acquisition* setup_acquisition( const config& );
hbridge*     setup_hbridge( const config& );
comms*       setup_comms();
debug_comms* setup_debug_comms();

}  // namespace fw
