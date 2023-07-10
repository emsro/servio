#include "control.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/conversion.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/drivers/hbridge.hpp"

#include <io.pb.h>

#pragma once

namespace fw
{

struct dispatcher
{
        hbridge&                                        hb;
        acquisition&                                    acquis;
        control&                                        ctl;
        metrics&                                        met;
        cfg_dispatcher&                                 cfg_disp;
        em::function_view< bool( const cfg_map* cfg ) > cfg_writer;
        converter&                                      conv;
        microseconds                                    now;
};

ServioToHost handle_message( dispatcher& dis, const HostToServio& msg );

}  // namespace fw
