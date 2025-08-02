#include "./dispatcher.hpp"

namespace servio::cfg
{

void dispatcher::apply( key key )
{
        switch ( key ) {
        case key::model:
        case key::id:
        case key::group_id:
                break;
        case key::encoder_mode:
                break;
        case key::quad_encoder_range:
                break;
        case key::position_low_angle:
        case key::position_high_angle:
                met.set_position_range( { m.position_low_angle, m.position_high_angle } );
                {
                        auto [min_val, max_val] = pos.get_position_range();
                        conv.set_position_cfg(
                            min_val, m.position_low_angle, max_val, m.position_high_angle );
                }
                break;
        case key::current_conv_scale:
        case key::current_conv_offset:
                conv.set_current_cfg( m.current_conv_scale, m.current_conv_offset );
                break;
        case key::temp_conv_scale:
        case key::temp_conv_offset:
                conv.set_temp_cfg( m.temp_conv_scale, m.temp_conv_offset );
                break;
        case key::voltage_conv_scale:
                conv.set_vcc_cfg( m.voltage_conv_scale );
                break;
        case key::invert_hbridge:
                motor.set_invert( m.invert_hbridge );
                break;
        case key::minimum_voltage:
                mon.set_minimum_voltage( m.minimum_voltage );
                break;
        case key::maximum_temperature:
                mon.set_maximum_temperature( m.maximum_temperature );
                break;
        case key::moving_detection_step:
                met.set_moving_step( m.moving_detection_step );
                break;
        default:
                break;
        }
        cfg::apply( ctl, m, key );
}

void dispatcher::full_apply()
{
        for ( auto key : map::keys )
                dispatcher::apply( key );
}

void apply( ctl::control& ctl, map const& m, key k )
{
        switch ( k ) {
        case key::current_loop_p:
        case key::current_loop_i:
        case key::current_loop_d:
                ctl.set_pid(
                    control_loop::CURRENT,
                    { m.current_loop_p, m.current_loop_i, m.current_loop_d } );
                break;
        case key::current_lim_min:
        case key::current_lim_max:
                ctl.set_limits( control_loop::CURRENT, { m.current_lim_min, m.current_lim_max } );
                break;
        case key::velocity_loop_p:
        case key::velocity_loop_i:
        case key::velocity_loop_d:
                ctl.set_pid(
                    control_loop::VELOCITY,
                    { m.velocity_loop_p, m.velocity_loop_i, m.velocity_loop_d } );
                break;
        case key::velocity_lim_min:
        case key::velocity_lim_max:
                ctl.set_limits(
                    control_loop::VELOCITY, { m.velocity_lim_min, m.velocity_lim_max } );
                break;
        case key::velocity_to_current_lim_scale:
                ctl.set_vel_to_curr_lim_scale( m.velocity_to_current_lim_scale );
                break;
        case key::position_loop_p:
        case key::position_loop_i:
        case key::position_loop_d:
                ctl.set_pid(
                    control_loop::POSITION,
                    { m.position_loop_p, m.position_loop_i, m.position_loop_d } );
                break;
        case key::position_lim_min:
        case key::position_lim_max:
                ctl.set_limits(
                    control_loop::POSITION, { m.position_lim_min, m.position_lim_max } );
                break;
        case key::position_to_velocity_lim_scale:
                ctl.set_pos_to_vel_lim_scale( m.position_to_velocity_lim_scale );
                break;
        case key::static_friction_scale:
        case key::static_friction_decay:
                ctl.set_static_friction( m.static_friction_scale, m.static_friction_decay );
                break;
        default:
                break;
        }
}

}  // namespace servio::cfg
