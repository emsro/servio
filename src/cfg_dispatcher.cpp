#include "cfg_dispatcher.hpp"

#include <emlabcpp/protocol/register_handler.h>

using handler = em::protocol::register_handler< cfg::map >;

void cfg_dispatcher::full_apply()
{
        for ( const cfg::key key : cfg::map::keys ) {
                apply( key );
        }
}

void cfg_dispatcher::apply( const cfg::key& key )
{
        switch ( key ) {
        case cfg::MODEL:
        case cfg::ID:
        case cfg::GROUP_ID:
                break;
        case cfg::POSITION_CONV_LOWER_SETPOINT_ANGLE:
        case cfg::POSITION_CONV_HIGHER_SETPOINT_ANGLE:
                c.met.set_position_range(
                    { map.get_val< cfg::POSITION_CONV_LOWER_SETPOINT_ANGLE >(),
                      map.get_val< cfg::POSITION_CONV_HIGHER_SETPOINT_ANGLE >() } );
                [[fallthrough]];
        case cfg::POSITION_CONV_LOWER_SETPOINT_VALUE:
        case cfg::POSITION_CONV_HIGHER_SETPOINT_VALUE:
                c.conv.set_position_cfg(
                    map.get_val< cfg::POSITION_CONV_LOWER_SETPOINT_VALUE >(),
                    map.get_val< cfg::POSITION_CONV_LOWER_SETPOINT_ANGLE >(),
                    map.get_val< cfg::POSITION_CONV_HIGHER_SETPOINT_VALUE >(),
                    map.get_val< cfg::POSITION_CONV_HIGHER_SETPOINT_ANGLE >() );
                break;
        case cfg::CURRENT_CONV_SCALE:
        case cfg::CURRENT_CONV_OFFSET:
                c.conv.set_current_cfg(
                    map.get_val< cfg::CURRENT_CONV_SCALE >(),
                    map.get_val< cfg::CURRENT_CONV_OFFSET >() );
                break;
        case cfg::TEMP_CONV_SCALE:
        case cfg::TEMP_CONV_OFFSET:
                c.conv.set_temp_cfg(
                    map.get_val< cfg::TEMP_CONV_SCALE >(), map.get_val< cfg::TEMP_CONV_OFFSET >() );
                break;
        case cfg::VOLTAGE_CONV_SCALE:
                c.conv.set_vcc_cfg( map.get_val< cfg::VOLTAGE_CONV_SCALE >() );
                break;
        case cfg::CURRENT_LOOP_P:
        case cfg::CURRENT_LOOP_I:
        case cfg::CURRENT_LOOP_D:
                c.ctl.set_pid(
                    control_loop::CURRENT,
                    { map.get_val< cfg::CURRENT_LOOP_P >(),
                      map.get_val< cfg::CURRENT_LOOP_I >(),
                      map.get_val< cfg::CURRENT_LOOP_D >() } );
                break;
        case cfg::CURRENT_LIM_MIN:
        case cfg::CURRENT_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::CURRENT,
                    { map.get_val< cfg::CURRENT_LIM_MIN >(),
                      map.get_val< cfg::CURRENT_LIM_MAX >() } );
                break;
        case cfg::VELOCITY_LOOP_P:
        case cfg::VELOCITY_LOOP_I:
        case cfg::VELOCITY_LOOP_D:
                c.ctl.set_pid(
                    control_loop::VELOCITY,
                    { map.get_val< cfg::VELOCITY_LOOP_P >(),
                      map.get_val< cfg::VELOCITY_LOOP_I >(),
                      map.get_val< cfg::VELOCITY_LOOP_D >() } );
                break;
        case cfg::VELOCITY_LIM_MIN:
        case cfg::VELOCITY_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::VELOCITY,
                    { map.get_val< cfg::VELOCITY_LIM_MIN >(),
                      map.get_val< cfg::VELOCITY_LIM_MAX >() } );
                break;
        case cfg::POSITION_LOOP_P:
        case cfg::POSITION_LOOP_I:
        case cfg::POSITION_LOOP_D:
                c.ctl.set_pid(
                    control_loop::POSITION,
                    { map.get_val< cfg::POSITION_LOOP_P >(),
                      map.get_val< cfg::POSITION_LOOP_I >(),
                      map.get_val< cfg::POSITION_LOOP_D >() } );
                break;
        case cfg::POSITION_LIM_MIN:
        case cfg::POSITION_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::POSITION,
                    { map.get_val< cfg::POSITION_LIM_MIN >(),
                      map.get_val< cfg::POSITION_LIM_MAX >() } );
                break;
        case cfg::STATIC_FRICTION_SCALE:
        case cfg::STATIC_FRICTION_DECAY:
                c.ctl.set_static_friction(
                    map.get_val< cfg::STATIC_FRICTION_SCALE >(),
                    map.get_val< cfg::STATIC_FRICTION_DECAY >() );
                break;
        case cfg::MINIMUM_VOLTAGE:
                c.mon.set_minimum_voltage( map.get_val< cfg::MINIMUM_VOLTAGE >() );
                break;
        case cfg::MAXIMUM_TEMPERATURE:
                c.mon.set_maximum_temperature( map.get_val< cfg::MAXIMUM_TEMPERATURE >() );
                break;
        case cfg::MOVING_DETECTION_STEP:
                c.met.set_moving_step( map.get_val< cfg::MOVING_DETECTION_STEP >() );
                break;
        }
}
