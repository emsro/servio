#include "../cfg/key.hpp"
#include "../iface/def.h"

#pragma once

namespace servio::core
{

using namespace avakar::literals;

consteval auto _build_iface_cfg_map()
{
        std::array< cfg::key, iface::cfg_key::keys.size() > res;
        auto f = [&]( iface::cfg_key idx, cfg::key k ) {
                res[idx.value()] = k;
        };
        f( "model"_a, cfg::MODEL );
        f( "id"_a, cfg::ID );
        f( "group_id"_a, cfg::GROUP_ID );
        f( "encoder_mode"_a, cfg::ENCODER_MODE );
        f( "position_lower_angle"_a, cfg::POSITION_LOW_ANGLE );
        f( "position_higher_angle"_a, cfg::POSITION_HIGH_ANGLE );
        f( "current_conv_scale"_a, cfg::CURRENT_CONV_SCALE );
        f( "current_conv_offset"_a, cfg::CURRENT_CONV_OFFSET );
        f( "temp_conv_scale"_a, cfg::TEMP_CONV_SCALE );
        f( "temp_conv_offset"_a, cfg::TEMP_CONV_OFFSET );
        f( "voltage_conv_scale"_a, cfg::VOLTAGE_CONV_SCALE );
        f( "invert_hbridge"_a, cfg::INVERT_HBRIDGE );
        f( "current_loop_p"_a, cfg::CURRENT_LOOP_P );
        f( "current_loop_i"_a, cfg::CURRENT_LOOP_I );
        f( "current_loop_d"_a, cfg::CURRENT_LOOP_D );
        f( "current_lim_min"_a, cfg::CURRENT_LIM_MIN );
        f( "current_lim_max"_a, cfg::CURRENT_LIM_MAX );
        f( "velocity_loop_p"_a, cfg::VELOCITY_LOOP_P );
        f( "velocity_loop_i"_a, cfg::VELOCITY_LOOP_I );
        f( "velocity_loop_d"_a, cfg::VELOCITY_LOOP_D );
        f( "velocity_lim_min"_a, cfg::VELOCITY_LIM_MIN );
        f( "velocity_lim_max"_a, cfg::VELOCITY_LIM_MAX );
        f( "velocity_to_current_lim_scale"_a, cfg::VELOCITY_TO_CURR_LIM_SCALE );
        f( "position_loop_p"_a, cfg::POSITION_LOOP_P );
        f( "position_loop_i"_a, cfg::POSITION_LOOP_I );
        f( "position_loop_d"_a, cfg::POSITION_LOOP_D );
        f( "position_lim_min"_a, cfg::POSITION_LIM_MIN );
        f( "position_lim_max"_a, cfg::POSITION_LIM_MAX );
        f( "position_to_velocity_lim_scale"_a, cfg::POSITION_TO_VEL_LIM_SCALE );
        f( "static_friction_scale"_a, cfg::STATIC_FRICTION_SCALE );
        f( "static_friction_decay"_a, cfg::STATIC_FRICTION_DECAY );
        f( "moving_detection_step"_a, cfg::MOVING_DETECTION_STEP );
        f( "minimum_voltage"_a, cfg::MINIMUM_VOLTAGE );
        f( "maximum_temperature"_a, cfg::MAXIMUM_TEMPERATURE );
        f( "quad_encoder_range"_a, cfg::QUAD_ENCD_RANGE );
        return res;
}

static constexpr auto iface_to_cfg = _build_iface_cfg_map();

}  // namespace servio::core
