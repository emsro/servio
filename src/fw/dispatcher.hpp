#include "control.hpp"
#include "drivers/acquisition.hpp"
#include "drivers/comms.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/conversion.hpp"

#include <io.pb.h>

#pragma once

namespace fw
{

struct dispatcher
{
        hbridge&        hb;
        acquisition&    acquis;
        control&        ctl;
        cfg_dispatcher& cfg_disp;
        converter&      conv;
        microseconds    now;
};

ServioToHost handle_message( dispatcher& dis, const HostToServio& msg );

}  // namespace fw
