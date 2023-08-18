#include "cfg_dispatcher.hpp"

#include <emlabcpp/protocol/register_handler.h>

using handler = em::protocol::register_handler< cfg_map >;

void cfg_dispatcher::full_apply()
{
        for ( const cfg_key key : cfg_map::keys ) {
                apply( key );
        }
}

void cfg_dispatcher::apply( const cfg_key& key )
{
        switch ( key ) {
        case MODEL:
        case ID:
        case GROUP_ID:
                break;
        case POSITION_CONV_LOWER_SETPOINT_ANGLE:
        case POSITION_CONV_HIGHER_SETPOINT_ANGLE:
                c.met.set_position_range(
                    { map.get_val< POSITION_CONV_LOWER_SETPOINT_ANGLE >(),
                      map.get_val< POSITION_CONV_HIGHER_SETPOINT_ANGLE >() } );
                [[fallthrough]];
        case POSITION_CONV_LOWER_SETPOINT_VALUE:
        case POSITION_CONV_HIGHER_SETPOINT_VALUE:
                c.conv.set_position_cfg(
                    map.get_val< POSITION_CONV_LOWER_SETPOINT_VALUE >(),
                    map.get_val< POSITION_CONV_LOWER_SETPOINT_ANGLE >(),
                    map.get_val< POSITION_CONV_HIGHER_SETPOINT_VALUE >(),
                    map.get_val< POSITION_CONV_HIGHER_SETPOINT_ANGLE >() );
                break;
        case CURRENT_CONV_SCALE:
        case CURRENT_CONV_OFFSET:
                c.conv.set_current_cfg(
                    map.get_val< CURRENT_CONV_SCALE >(), map.get_val< CURRENT_CONV_OFFSET >() );
                break;
        case TEMP_CONV_SCALE:
        case TEMP_CONV_OFFSET:
                c.conv.set_temp_cfg(
                    map.get_val< TEMP_CONV_SCALE >(), map.get_val< TEMP_CONV_OFFSET >() );
                break;
        case VOLTAGE_CONV_SCALE:
                c.conv.set_vcc_cfg( map.get_val< VOLTAGE_CONV_SCALE >() );
                break;
        case CURRENT_LOOP_P:
        case CURRENT_LOOP_I:
        case CURRENT_LOOP_D:
                c.ctl.set_pid(
                    control_loop::CURRENT,
                    { map.get_val< CURRENT_LOOP_P >(),
                      map.get_val< CURRENT_LOOP_I >(),
                      map.get_val< CURRENT_LOOP_D >() } );
                break;
        case CURRENT_LIM_MIN:
        case CURRENT_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::CURRENT,
                    { map.get_val< CURRENT_LIM_MIN >(), map.get_val< CURRENT_LIM_MAX >() } );
                break;
        case VELOCITY_LOOP_P:
        case VELOCITY_LOOP_I:
        case VELOCITY_LOOP_D:
                c.ctl.set_pid(
                    control_loop::VELOCITY,
                    { map.get_val< VELOCITY_LOOP_P >(),
                      map.get_val< VELOCITY_LOOP_I >(),
                      map.get_val< VELOCITY_LOOP_D >() } );
                break;
        case VELOCITY_LIM_MIN:
        case VELOCITY_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::VELOCITY,
                    { map.get_val< VELOCITY_LIM_MIN >(), map.get_val< VELOCITY_LIM_MAX >() } );
                break;
        case POSITION_LOOP_P:
        case POSITION_LOOP_I:
        case POSITION_LOOP_D:
                c.ctl.set_pid(
                    control_loop::POSITION,
                    { map.get_val< POSITION_LOOP_P >(),
                      map.get_val< POSITION_LOOP_I >(),
                      map.get_val< POSITION_LOOP_D >() } );
                break;
        case POSITION_LIM_MIN:
        case POSITION_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::POSITION,
                    { map.get_val< POSITION_LIM_MIN >(), map.get_val< POSITION_LIM_MAX >() } );
                break;
        case STATIC_FRICTION_SCALE:
        case STATIC_FRICTION_DECAY:
                c.ctl.set_static_friction(
                    map.get_val< STATIC_FRICTION_SCALE >(),
                    map.get_val< STATIC_FRICTION_DECAY >() );
                break;
        case MINIMUM_VOLTAGE:
                c.mon.set_minimum_voltage( map.get_val< MINIMUM_VOLTAGE >() );
                break;
        case MAXIMUM_TEMPERATURE:
                c.mon.set_maximum_temperature( map.get_val< MAXIMUM_TEMPERATURE >() );
                break;
        case MOVING_DETECTION_STEP:
                c.met.set_moving_step( map.get_val< MOVING_DETECTION_STEP >() );
                break;
        }
}
