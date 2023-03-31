#include "control.hpp"
#include "drivers/acquisition.hpp"
#include "drivers/comms.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/conversion.hpp"

#pragma once

namespace fw
{

struct dispatcher
{
        comms&          comm;
        hbridge&        hb;
        acquisition&    acquis;
        control&        ctl;
        cfg_dispatcher& cfg_disp;
        converter&      conv;
        microseconds    now;

        void handle_message( const switch_to_power_command& cmd )
        {
                ctl.switch_to_power_control( cmd.value );
        }

        void handle_message( const switch_to_current_command& cmd )
        {
                ctl.switch_to_current_control( now, cmd.value );
        }

        void handle_message( const switch_to_velocity_command& cmd )
        {
                ctl.switch_to_velocity_control( now, cmd.value );
        }

        void handle_message( const switch_to_position_command& cmd )
        {
                ctl.switch_to_position_control( now, cmd.value );
        }

        void handle_message( const get_current_command& )
        {
                float curr = current( conv, acquis, hb );
                comm.send( get_current_response{ curr } );
        }

        void handle_message( const get_vcc_command& )
        {
                comm.send( get_vcc_response{ conv.vcc.convert( acquis.get_vcc() ) } );
        }

        void handle_message( const get_temp_command& )
        {
                comm.send( get_temp_response{ conv.temp.convert( acquis.get_temp() ) } );
        }

        void handle_message( const get_position_command& )
        {
                float pos = position( conv, acquis );
                comm.send( get_position_response{ pos } );
        }

        void handle_message( const set_config& req )
        {
                cfg_disp.set( req.key, req.msg );
        }

        void handle_message( const get_config_request& req )
        {
                comm.send( get_config_response{ .msg = cfg_disp.get( req.key ) } );
        }
};

}  // namespace fw
