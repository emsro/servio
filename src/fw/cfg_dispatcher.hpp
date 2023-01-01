#include "config.hpp"

#include <emlabcpp/protocol/register_handler.h>

#pragma once

namespace fw
{

struct cfg_dispatcher
{

    cfg_map&     map;
    acquisition& acq;

    using handler = em::protocol::register_handler< cfg_map >;

    void operator()( const cfg_keyval& kv )
    {
        handler::insert( map, kv.key, kv.msg );

        switch ( kv.key ) {
            case cfg_key::REVERSED:
                break;
            case cfg_key::POSITION_CONV_LOWER_SETPOINT_VALUE:
            case cfg_key::POSITION_CONV_LOWER_SETPOINT_ANGLE:
            case cfg_key::POSITION_CONV_HIGHER_SETPOINT_VALUE:
            case cfg_key::POSITION_CONV_HIGHER_SETPOINT_ANGLE:
                acq.set_position_cfg(
                    map.get_val< cfg_key::POSITION_CONV_LOWER_SETPOINT_VALUE >(),
                    map.get_val< cfg_key::POSITION_CONV_LOWER_SETPOINT_ANGLE >(),
                    map.get_val< cfg_key::POSITION_CONV_HIGHER_SETPOINT_VALUE >(),
                    map.get_val< cfg_key::POSITION_CONV_HIGHER_SETPOINT_ANGLE >() );
                break;
            case cfg_key::CURRENT_CONV_SCALE:
            case cfg_key::CURRENT_CONV_OFFSET:
                acq.set_current_cfg(
                    map.get_val< cfg_key::CURRENT_CONV_SCALE >(),
                    map.get_val< cfg_key::CURRENT_CONV_OFFSET >() );
                break;
            case cfg_key::TEMP_CONV_SCALE:
            case cfg_key::TEMP_CONV_OFFSET:
                acq.set_temp_cfg(
                    map.get_val< cfg_key::TEMP_CONV_SCALE >(),
                    map.get_val< cfg_key::TEMP_CONV_OFFSET >() );
                break;
            case cfg_key::VOLTAGE_CONV_SCALE:
                acq.set_vcc_cfg( map.get_val< cfg_key::VOLTAGE_CONV_SCALE >() );
                break;
        }
    }
};

}  // namespace fw
