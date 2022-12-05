#include "control.hpp"
#include "drivers/acquisition.hpp"
#include "drivers/comms.hpp"

#pragma once

namespace fw
{

struct dispatcher
{
    comms&                    comm;
    acquisition&              acquistion;
    control&                  ctl;
    std::chrono::milliseconds now;

    void handle_message( switch_to_power_command cmd )
    {
        ctl.switch_to_power_control( cmd.value );
    }

    void handle_message( switch_to_current_command cmd )
    {
        ctl.switch_to_current_control( now, cmd.value );
    }

    void handle_message( switch_to_velocity_command cmd )
    {
        ctl.switch_to_velocity_control( now, cmd.value );
    }

    void handle_message( switch_to_position_command cmd )
    {
        ctl.switch_to_position_control( now, cmd.value );
    }

    void handle_message( measure_current_command )
    {
        em::static_vector< uint16_t, 128 > tmp;

        auto buff = acquistion.get_current_reading();
        std::copy( buff.begin(), buff.end(), std::back_inserter( tmp ) );

        comm.transmit( measure_current_response( tmp ) );
    }

    void handle_message( get_current_command )
    {
        comm.transmit( get_current_response{ acquistion.get_current() } );
    }

    void handle_message( get_vcc_command )
    {
        comm.transmit( get_vcc_response{ acquistion.get_vcc() } );
    }

    void handle_message( get_temp_command )
    {
        comm.transmit( get_temp_response{ acquistion.get_temp() } );
    }

    void handle_message( get_position_command )
    {
        comm.transmit( get_position_response{ acquistion.get_position() } );
    }
};

}  // namespace fw
