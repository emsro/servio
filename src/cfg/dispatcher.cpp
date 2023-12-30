#include "cfg/dispatcher.hpp"

#include "cfg/key.hpp"

#include <emlabcpp/protocol/register_handler.h>

namespace servio::cfg
{

using handler = em::protocol::register_handler< map >;

void dispatcher::full_apply()
{
        for ( const auto key : map::keys ) {
                apply( key );
        }
}

void dispatcher::apply( const key& key )
{
        switch ( key ) {
        case MODEL:
        case ID:
        case GROUP_ID:
                break;
        case POSITION_CONV_LOW_ANGLE:
        case POSITION_CONV_HIGH_ANGLE:
                c.met.set_position_range(
                    { m.get_val< POSITION_CONV_LOW_ANGLE >(),
                      m.get_val< POSITION_CONV_HIGH_ANGLE >() } );
                [[fallthrough]];
        case POSITION_CONV_LOW_VALUE:
        case POSITION_CONV_HIGH_VALUE:
                c.conv.set_position_cfg(
                    m.get_val< POSITION_CONV_LOW_VALUE >(),
                    m.get_val< POSITION_CONV_LOW_ANGLE >(),
                    m.get_val< POSITION_CONV_HIGH_VALUE >(),
                    m.get_val< POSITION_CONV_HIGH_ANGLE >() );
                break;
        case CURRENT_CONV_SCALE:
        case CURRENT_CONV_OFFSET:
                c.conv.set_current_cfg(
                    m.get_val< CURRENT_CONV_SCALE >(), m.get_val< CURRENT_CONV_OFFSET >() );
                break;
        case TEMP_CONV_SCALE:
        case TEMP_CONV_OFFSET:
                c.conv.set_temp_cfg(
                    m.get_val< TEMP_CONV_SCALE >(), m.get_val< TEMP_CONV_OFFSET >() );
                break;
        case VOLTAGE_CONV_SCALE:
                c.conv.set_vcc_cfg( m.get_val< VOLTAGE_CONV_SCALE >() );
                break;
        case MINIMUM_VOLTAGE:
                c.mon.set_minimum_voltage( m.get_val< MINIMUM_VOLTAGE >() );
                break;
        case MAXIMUM_TEMPERATURE:
                c.mon.set_maximum_temperature( m.get_val< MAXIMUM_TEMPERATURE >() );
                break;
        case MOVING_DETECTION_STEP:
                c.met.set_moving_step( m.get_val< MOVING_DETECTION_STEP >() );
                break;
        default:
                break;
        }
        ::servio::cfg::apply( c.ctl, m, key );
}

void apply( control& ctl, const map& m, const key& key )
{

        switch ( key ) {
        case CURRENT_LOOP_P:
        case CURRENT_LOOP_I:
        case CURRENT_LOOP_D:
                ctl.set_pid(
                    control_loop::CURRENT,
                    { m.get_val< CURRENT_LOOP_P >(),
                      m.get_val< CURRENT_LOOP_I >(),
                      m.get_val< CURRENT_LOOP_D >() } );
                break;
        case CURRENT_LIM_MIN:
        case CURRENT_LIM_MAX:
                ctl.set_limits(
                    control_loop::CURRENT,
                    { m.get_val< CURRENT_LIM_MIN >(), m.get_val< CURRENT_LIM_MAX >() } );
                break;
        case VELOCITY_LOOP_P:
        case VELOCITY_LOOP_I:
        case VELOCITY_LOOP_D:
                ctl.set_pid(
                    control_loop::VELOCITY,
                    { m.get_val< VELOCITY_LOOP_P >(),
                      m.get_val< VELOCITY_LOOP_I >(),
                      m.get_val< VELOCITY_LOOP_D >() } );
                break;
        case VELOCITY_LIM_MIN:
        case VELOCITY_LIM_MAX:
                ctl.set_limits(
                    control_loop::VELOCITY,
                    { m.get_val< VELOCITY_LIM_MIN >(), m.get_val< VELOCITY_LIM_MAX >() } );
                break;
        case VELOCITY_TO_CURR_LIM_SCALE:
                ctl.set_vel_to_curr_lim_scale( m.get_val< VELOCITY_TO_CURR_LIM_SCALE >() );
                break;
        case POSITION_LOOP_P:
        case POSITION_LOOP_I:
        case POSITION_LOOP_D:
                ctl.set_pid(
                    control_loop::POSITION,
                    { m.get_val< POSITION_LOOP_P >(),
                      m.get_val< POSITION_LOOP_I >(),
                      m.get_val< POSITION_LOOP_D >() } );
                break;
        case POSITION_LIM_MIN:
        case POSITION_LIM_MAX:
                ctl.set_limits(
                    control_loop::POSITION,
                    { m.get_val< POSITION_LIM_MIN >(), m.get_val< POSITION_LIM_MAX >() } );
                break;
        case POSITION_TO_VEL_LIM_SCALE:
                ctl.set_pos_to_vel_lim_scale( m.get_val< POSITION_TO_VEL_LIM_SCALE >() );
                break;
        case STATIC_FRICTION_SCALE:
        case STATIC_FRICTION_DECAY:
                ctl.set_static_friction(
                    m.get_val< STATIC_FRICTION_SCALE >(), m.get_val< STATIC_FRICTION_DECAY >() );
                break;
        default:
                break;
        }
}

}  // namespace servio::cfg
