#include "./key.hpp"
#include "./map.hpp"

#pragma once

namespace servio::cfg
{

consteval map get_default_config()
{
        return map{
            reg< MODEL, model_name >{ "no model" },
            reg< ID, uint32_t >{ 0 },
            reg< GROUP_ID, uint32_t >{ 0 },
            reg< ENCODER_MODE, encoder_mode >{ ENC_MODE_QUAD },
            reg< POSITION_LOW_ANGLE, float >{ 0.F },
            reg< POSITION_HIGH_ANGLE, float >{ 2 * pi },
            reg< CURRENT_CONV_SCALE, float >{ 1.F },
            reg< CURRENT_CONV_OFFSET, float >{ 0.F },
            reg< TEMP_CONV_SCALE, float >{ 1.F },
            reg< TEMP_CONV_OFFSET, float >{ 0.F },
            reg< VOLTAGE_CONV_SCALE, float >{ 1.F },
            reg< INVERT_HBRIDGE, bool >{ false },
            reg< CURRENT_LOOP_P, float >{ 0.03125F },
            reg< CURRENT_LOOP_I, float >{ 0.000006F },
            reg< CURRENT_LOOP_D, float >{ 0.F },
            reg< CURRENT_LIM_MIN, float >{ -2.F },
            reg< CURRENT_LIM_MAX, float >{ 2.F },
            reg< VELOCITY_LOOP_P, float >{ 0.02F },
            reg< VELOCITY_LOOP_I, float >{ 0.0000002F },
            reg< VELOCITY_LOOP_D, float >{ 0.02F },
            reg< VELOCITY_LIM_MIN, float >{ -3.F },
            reg< VELOCITY_LIM_MAX, float >{ 3.F },
            reg< VELOCITY_TO_CURR_LIM_SCALE, float >{ 2.F },
            reg< POSITION_LOOP_P, float >{ 0.2F },
            reg< POSITION_LOOP_I, float >{ 0.00000002F },
            reg< POSITION_LOOP_D, float >{ 0.F },
            reg< POSITION_LIM_MIN, float >{ 0.1F },
            reg< POSITION_LIM_MAX, float >{ 2 * pi - 0.1F },
            reg< POSITION_TO_VEL_LIM_SCALE, float >{ 2.F },
            reg< STATIC_FRICTION_SCALE, float >{ 3.F },
            reg< STATIC_FRICTION_DECAY, float >{ 1.F },
            reg< MINIMUM_VOLTAGE, float >{ 6.F },
            reg< MAXIMUM_TEMPERATURE, float >{ 80.F },
            reg< MOVING_DETECTION_STEP, float >{ 0.05F },
            reg< QUAD_ENCD_RANGE, uint32_t >{ 2048 },
        };
}
}  // namespace servio::cfg
