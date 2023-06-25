#include "control.hpp"
#include "drivers/acquisition.hpp"
#include "drivers/comms.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/conversion.hpp"
#include "fw/servio_pb.hpp"

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

        ServioToHost handle_message( const HostToServio& msg );

        ServioToHost handle_message( const HostToServio_SetMode& msg );

        ServioToHost handle_message( const HostToServio_GetProperty& msg );

        ServioToHost handle_message( const Config& req );

        ServioToHost handle_message( const HostToServio_GetConfig& req );
};

}  // namespace fw
