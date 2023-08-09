#include "config.hpp"

#pragma once

namespace cfg
{

consteval cfg_map get_default_config()
{
        return cfg_map{
            cfg_reg< MODEL, model_name >{ "no model" },
            cfg_reg< ID, uint32_t >{ 0 },
            cfg_reg< GROUP_ID, uint32_t >{ 0 },
            cfg_reg< POSITION_CONV_LOWER_SETPOINT_VALUE, uint32_t >{ 0 },
            cfg_reg< POSITION_CONV_LOWER_SETPOINT_ANGLE, float >{ 0.F },
            cfg_reg< POSITION_CONV_HIGHER_SETPOINT_VALUE, uint32_t >{ 4096 },
            cfg_reg< POSITION_CONV_HIGHER_SETPOINT_ANGLE, float >{ 2 * pi },
            cfg_reg< CURRENT_CONV_SCALE, float >{ 1.F },
            cfg_reg< CURRENT_CONV_OFFSET, float >{ 0.F },
            cfg_reg< TEMP_CONV_SCALE, float >{ 1.F },
            cfg_reg< TEMP_CONV_OFFSET, float >{ 0.F },
            cfg_reg< VOLTAGE_CONV_SCALE, float >{ 1.F },
            cfg_reg< CURRENT_LOOP_P, float >{ 1024.F * 32.F },
            cfg_reg< CURRENT_LOOP_I, float >{ 4.F },
            cfg_reg< CURRENT_LOOP_D, float >{ 0.F },
            cfg_reg< CURRENT_LIM_MIN, float >{ -2.F },
            cfg_reg< CURRENT_LIM_MAX, float >{ 2.F },
            cfg_reg< VELOCITY_LOOP_P, float >{ 0.02F },
            cfg_reg< VELOCITY_LOOP_I, float >{ 0.0000002F },
            cfg_reg< VELOCITY_LOOP_D, float >{ 0.02F },
            cfg_reg< VELOCITY_LIM_MIN, float >{ -3.F },
            cfg_reg< VELOCITY_LIM_MAX, float >{ 3.F },
            cfg_reg< POSITION_LOOP_P, float >{ 0.2F },
            cfg_reg< POSITION_LOOP_I, float >{ 0.0000002F },
            cfg_reg< POSITION_LOOP_D, float >{ 0.F },
            cfg_reg< POSITION_LIM_MIN, float >{ 0.1F },
            cfg_reg< POSITION_LIM_MAX, float >{ 2 * pi - 0.1F },
            cfg_reg< STATIC_FRICTION_SCALE, float >{ 3.F },
            cfg_reg< STATIC_FRICTION_DECAY, float >{ 1.F },
            cfg_reg< MINIMUM_VOLTAGE, float >{ 6.F },
            cfg_reg< MAXIMUM_TEMPERATURE, float >{ 80.F },
            cfg_reg< MOVING_DETECTION_STEP, float >{ 0.05F },
        };
}
}  // namespace cfg
