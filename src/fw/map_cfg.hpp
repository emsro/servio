#include "cfg/key.hpp"

#include <io.pb.h>

#pragma once

namespace servio::fw
{

template < typename Cfg, typename UnaryFunction >
[[nodiscard]] bool map_cfg( uint32_t key, Cfg& cfg, UnaryFunction&& f )
{
        bool found_key = true;
        switch ( key ) {
        case Config_model_tag:
                f.template operator()< cfg::MODEL >( cfg.model );
                break;
        case Config_id_tag:
                f.template operator()< cfg::ID >( cfg.id );
                break;
        case Config_group_id_tag:
                f.template operator()< cfg::GROUP_ID >( cfg.group_id );
                break;
        case Config_encoder_mode_tag:
                f.template operator()< cfg::ENCODER_MODE >( cfg.encoder_mode );
                break;
        case Config_position_conv_lower_setpoint_value_tag:
                f.template operator()< cfg::POSITION_CONV_LOW_VALUE >(
                    cfg.position_conv_lower_setpoint_value );
                break;
        case Config_position_conv_lower_setpoint_angle_tag:
                f.template operator()< cfg::POSITION_CONV_LOW_ANGLE >(
                    cfg.position_conv_lower_setpoint_angle );
                break;
        case Config_position_conv_higher_setpoint_value_tag:
                f.template operator()< cfg::POSITION_CONV_HIGH_VALUE >(
                    cfg.position_conv_higher_setpoint_value );
                break;
        case Config_position_conv_higher_setpoint_angle_tag:
                f.template operator()< cfg::POSITION_CONV_HIGH_ANGLE >(
                    cfg.position_conv_higher_setpoint_angle );
                break;
        case Config_current_conv_scale_tag:
                f.template operator()< cfg::CURRENT_CONV_SCALE >( cfg.current_conv_scale );
                break;
        case Config_current_conv_offset_tag:
                f.template operator()< cfg::CURRENT_CONV_OFFSET >( cfg.current_conv_offset );
                break;
        case Config_temp_conv_scale_tag:
                f.template operator()< cfg::TEMP_CONV_SCALE >( cfg.temp_conv_scale );
                break;
        case Config_temp_conv_offset_tag:
                f.template operator()< cfg::TEMP_CONV_OFFSET >( cfg.temp_conv_offset );
                break;
        case Config_voltage_conv_scale_tag:
                f.template operator()< cfg::VOLTAGE_CONV_SCALE >( cfg.voltage_conv_scale );
                break;
        case Config_invert_hbridge_tag:
                f.template operator()< cfg::INVERT_HBRIDGE >( cfg.invert_hbridge );
                break;
        case Config_current_loop_p_tag:
                f.template operator()< cfg::CURRENT_LOOP_P >( cfg.current_loop_p );
                break;
        case Config_current_loop_i_tag:
                f.template operator()< cfg::CURRENT_LOOP_I >( cfg.current_loop_i );
                break;
        case Config_current_loop_d_tag:
                f.template operator()< cfg::CURRENT_LOOP_D >( cfg.current_loop_d );
                break;
        case Config_current_lim_min_tag:
                f.template operator()< cfg::CURRENT_LIM_MIN >( cfg.current_lim_min );
                break;
        case Config_current_lim_max_tag:
                f.template operator()< cfg::CURRENT_LIM_MAX >( cfg.current_lim_max );
                break;
        case Config_velocity_loop_p_tag:
                f.template operator()< cfg::VELOCITY_LOOP_P >( cfg.velocity_loop_p );
                break;
        case Config_velocity_loop_i_tag:
                f.template operator()< cfg::VELOCITY_LOOP_I >( cfg.velocity_loop_i );
                break;
        case Config_velocity_loop_d_tag:
                f.template operator()< cfg::VELOCITY_LOOP_D >( cfg.velocity_loop_d );
                break;
        case Config_velocity_lim_min_tag:
                f.template operator()< cfg::VELOCITY_LIM_MIN >( cfg.velocity_lim_min );
                break;
        case Config_velocity_lim_max_tag:
                f.template operator()< cfg::VELOCITY_LIM_MAX >( cfg.velocity_lim_max );
                break;
        case Config_velocity_to_current_lim_scale_tag:
                f.template operator()< cfg::VELOCITY_TO_CURR_LIM_SCALE >(
                    cfg.velocity_to_current_lim_scale );
                break;
        case Config_position_loop_p_tag:
                f.template operator()< cfg::POSITION_LOOP_P >( cfg.position_loop_p );
                break;
        case Config_position_loop_i_tag:
                f.template operator()< cfg::POSITION_LOOP_I >( cfg.position_loop_i );
                break;
        case Config_position_loop_d_tag:
                f.template operator()< cfg::POSITION_LOOP_D >( cfg.position_loop_d );
                break;
        case Config_position_lim_min_tag:
                f.template operator()< cfg::POSITION_LIM_MIN >( cfg.position_lim_min );
                break;
        case Config_position_lim_max_tag:
                f.template operator()< cfg::POSITION_LIM_MAX >( cfg.position_lim_max );
                break;
        case Config_position_to_velocity_lim_scale_tag:
                f.template operator()< cfg::POSITION_TO_VEL_LIM_SCALE >(
                    cfg.position_to_velocity_lim_scale );
                break;
        case Config_static_friction_scale_tag:
                f.template operator()< cfg::STATIC_FRICTION_SCALE >( cfg.static_friction_scale );
                break;
        case Config_static_friction_decay_tag:
                f.template operator()< cfg::STATIC_FRICTION_DECAY >( cfg.static_friction_decay );
                break;
        case Config_minimum_voltage_tag:
                f.template operator()< cfg::MINIMUM_VOLTAGE >( cfg.minimum_voltage );
                break;
        case Config_maximum_temperature_tag:
                f.template operator()< cfg::MAXIMUM_TEMPERATURE >( cfg.maximum_temperature );
                break;
        case Config_moving_detection_step_tag:
                f.template operator()< cfg::MOVING_DETECTION_STEP >( cfg.moving_detection_step );
                break;
        default:
                found_key = false;
        }
        return found_key;
}

}  // namespace servio::fw
