#include "config.hpp"

#include <emlabcpp/protocol/register_handler.h>

#pragma once

namespace fw
{

struct cfg_dispatcher
{

    cfg_map&     map;
    acquisition& acq;
    control&     ctl;

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
            case cfg_key::CONTROL_CURRENT_LOOP_P:
            case cfg_key::CONTROL_CURRENT_LOOP_I:
            case cfg_key::CONTROL_CURRENT_LOOP_D:
                ctl.set_pid(
                    control_loop::CURRENT,
                    map.get_val< cfg_key::CONTROL_CURRENT_LOOP_P >(),
                    map.get_val< cfg_key::CONTROL_CURRENT_LOOP_I >(),
                    map.get_val< cfg_key::CONTROL_CURRENT_LOOP_D >() );
                break;
            case cfg_key::CONTROL_CURRENT_LIM_MIN:
            case cfg_key::CONTROL_CURRENT_LIM_MAX:
                ctl.set_limits(
                    control_loop::CURRENT,
                    { map.get_val< cfg_key::CONTROL_CURRENT_LIM_MIN >(),
                      map.get_val< cfg_key::CONTROL_CURRENT_LIM_MAX >() } );
                break;
            case cfg_key::CONTROL_VELOCITY_LOOP_P:
            case cfg_key::CONTROL_VELOCITY_LOOP_I:
            case cfg_key::CONTROL_VELOCITY_LOOP_D:
                ctl.set_pid(
                    control_loop::VELOCITY,
                    map.get_val< cfg_key::CONTROL_VELOCITY_LOOP_P >(),
                    map.get_val< cfg_key::CONTROL_VELOCITY_LOOP_I >(),
                    map.get_val< cfg_key::CONTROL_VELOCITY_LOOP_D >() );
                break;
            case cfg_key::CONTROL_VELOCITY_LIM_MIN:
            case cfg_key::CONTROL_VELOCITY_LIM_MAX:
                ctl.set_limits(
                    control_loop::VELOCITY,
                    { map.get_val< cfg_key::CONTROL_VELOCITY_LIM_MIN >(),
                      map.get_val< cfg_key::CONTROL_VELOCITY_LIM_MAX >() } );
                break;
            case cfg_key::CONTROL_POSITION_LOOP_P:
            case cfg_key::CONTROL_POSITION_LOOP_I:
            case cfg_key::CONTROL_POSITION_LOOP_D:
                ctl.set_pid(
                    control_loop::POSITION,
                    map.get_val< cfg_key::CONTROL_POSITION_LOOP_P >(),
                    map.get_val< cfg_key::CONTROL_POSITION_LOOP_I >(),
                    map.get_val< cfg_key::CONTROL_POSITION_LOOP_D >() );
                break;
            case cfg_key::CONTROL_POSITION_LIM_MIN:
            case cfg_key::CONTROL_POSITION_LIM_MAX:
                ctl.set_limits(
                    control_loop::POSITION,
                    { map.get_val< cfg_key::CONTROL_POSITION_LIM_MIN >(),
                      map.get_val< cfg_key::CONTROL_POSITION_LIM_MAX >() } );
                break;
        }
    }
};

}  // namespace fw
