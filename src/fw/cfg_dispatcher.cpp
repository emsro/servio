

#include "fw/cfg_dispatcher.hpp"

#include <emlabcpp/protocol/register_handler.h>

namespace fw
{

using handler = em::protocol::register_handler< cfg_map >;

cfg_value_message cfg_dispatcher::get( const cfg_key& key )
{
        return handler::select( map, key );
}

void cfg_dispatcher::set( const cfg_key& key, const cfg_value_message& msg )
{
        handler::insert( map, key, msg );
        apply( key );
}

void cfg_dispatcher::full_apply()
{
        for ( cfg_key key : map.get_keys() ) {
                apply( key );
        }
}

void cfg_dispatcher::apply( const cfg_key& key )
{
        switch ( key ) {
        case REVERSED:
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
        case CONTROL_CURRENT_LOOP_P:
        case CONTROL_CURRENT_LOOP_I:
        case CONTROL_CURRENT_LOOP_D:
                c.ctl.set_pid(
                    control_loop::CURRENT,
                    { map.get_val< CONTROL_CURRENT_LOOP_P >(),
                      map.get_val< CONTROL_CURRENT_LOOP_I >(),
                      map.get_val< CONTROL_CURRENT_LOOP_D >() } );
                break;
        case CONTROL_CURRENT_LIM_MIN:
        case CONTROL_CURRENT_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::CURRENT,
                    { map.get_val< CONTROL_CURRENT_LIM_MIN >(),
                      map.get_val< CONTROL_CURRENT_LIM_MAX >() } );
                break;
        case CONTROL_VELOCITY_LOOP_P:
        case CONTROL_VELOCITY_LOOP_I:
        case CONTROL_VELOCITY_LOOP_D:
                c.ctl.set_pid(
                    control_loop::VELOCITY,
                    { map.get_val< CONTROL_VELOCITY_LOOP_P >(),
                      map.get_val< CONTROL_VELOCITY_LOOP_I >(),
                      map.get_val< CONTROL_VELOCITY_LOOP_D >() } );
                break;
        case CONTROL_VELOCITY_LIM_MIN:
        case CONTROL_VELOCITY_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::VELOCITY,
                    { map.get_val< CONTROL_VELOCITY_LIM_MIN >(),
                      map.get_val< CONTROL_VELOCITY_LIM_MAX >() } );
                break;
        case CONTROL_POSITION_LOOP_P:
        case CONTROL_POSITION_LOOP_I:
        case CONTROL_POSITION_LOOP_D:
                c.ctl.set_pid(
                    control_loop::POSITION,
                    { map.get_val< CONTROL_POSITION_LOOP_P >(),
                      map.get_val< CONTROL_POSITION_LOOP_I >(),
                      map.get_val< CONTROL_POSITION_LOOP_D >() } );
                break;
        case CONTROL_POSITION_LIM_MIN:
        case CONTROL_POSITION_LIM_MAX:
                c.ctl.set_limits(
                    control_loop::POSITION,
                    { map.get_val< CONTROL_POSITION_LIM_MIN >(),
                      map.get_val< CONTROL_POSITION_LIM_MAX >() } );
                break;
        case MINIMUM_VOLTAGE:
                c.mon.set_minimum_voltage( map.get_val< MINIMUM_VOLTAGE >() );
                break;
        case MAXIMUM_TEMPERATURE:
                c.mon.set_maximum_temperature( map.get_val< MAXIMUM_TEMPERATURE >() );
                break;
        }
}
}  // namespace fw
