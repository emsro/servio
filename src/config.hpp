#include "base.hpp"

#include <emlabcpp/protocol/register_map.h>

#pragma once

namespace em = emlabcpp;

enum cfg_key : uint16_t
{
        REVERSED                            = 0x00,
        POSITION_CONV_LOWER_SETPOINT_VALUE  = 0x04,
        POSITION_CONV_LOWER_SETPOINT_ANGLE  = 0x08,
        POSITION_CONV_HIGHER_SETPOINT_VALUE = 0x0c,
        POSITION_CONV_HIGHER_SETPOINT_ANGLE = 0x10,
        CURRENT_CONV_SCALE                  = 0x14,
        CURRENT_CONV_OFFSET                 = 0x18,
        TEMP_CONV_SCALE                     = 0x1c,
        TEMP_CONV_OFFSET                    = 0x20,
        VOLTAGE_CONV_SCALE                  = 0x24,
        CONTROL_CURRENT_LOOP_P              = 0x28,
        CONTROL_CURRENT_LOOP_I              = 0x2c,
        CONTROL_CURRENT_LOOP_D              = 0x30,
        CONTROL_CURRENT_LIM_MIN             = 0x34,
        CONTROL_CURRENT_LIM_MAX             = 0x38,
        CONTROL_VELOCITY_LOOP_P             = 0x3c,
        CONTROL_VELOCITY_LOOP_I             = 0x40,
        CONTROL_VELOCITY_LOOP_D             = 0x44,
        CONTROL_VELOCITY_LIM_MIN            = 0x48,
        CONTROL_VELOCITY_LIM_MAX            = 0x4c,
        CONTROL_POSITION_LOOP_P             = 0x50,
        CONTROL_POSITION_LOOP_I             = 0x54,
        CONTROL_POSITION_LOOP_D             = 0x58,
        CONTROL_POSITION_LIM_MIN            = 0x5c,
        CONTROL_POSITION_LIM_MAX            = 0x60,
        MINIMUM_VOLTAGE                     = 0x64,
        MAXIMUM_TEMPERATURE                 = 0x68,
};

template < auto Key, typename T >
using cfg_reg = em::protocol::register_pair< Key, T >;

// TODO: write a check for reg_map that confirms that config values does not
// overlap
using cfg_map = em::protocol::register_map<
    std::endian::little,
    cfg_reg< REVERSED, bool >,
    cfg_reg< POSITION_CONV_LOWER_SETPOINT_VALUE, uint16_t >,
    cfg_reg< POSITION_CONV_LOWER_SETPOINT_ANGLE, float >,
    cfg_reg< POSITION_CONV_HIGHER_SETPOINT_VALUE, uint16_t >,
    cfg_reg< POSITION_CONV_HIGHER_SETPOINT_ANGLE, float >,
    cfg_reg< CURRENT_CONV_SCALE, float >,
    cfg_reg< CURRENT_CONV_OFFSET, float >,
    cfg_reg< TEMP_CONV_SCALE, float >,
    cfg_reg< TEMP_CONV_OFFSET, float >,
    cfg_reg< VOLTAGE_CONV_SCALE, float >,
    cfg_reg< CONTROL_CURRENT_LOOP_P, float >,
    cfg_reg< CONTROL_CURRENT_LOOP_I, float >,
    cfg_reg< CONTROL_CURRENT_LOOP_D, float >,
    cfg_reg< CONTROL_CURRENT_LIM_MIN, float >,
    cfg_reg< CONTROL_CURRENT_LIM_MAX, float >,
    cfg_reg< CONTROL_VELOCITY_LOOP_P, float >,
    cfg_reg< CONTROL_VELOCITY_LOOP_I, float >,
    cfg_reg< CONTROL_VELOCITY_LOOP_D, float >,
    cfg_reg< CONTROL_VELOCITY_LIM_MIN, float >,
    cfg_reg< CONTROL_VELOCITY_LIM_MAX, float >,
    cfg_reg< CONTROL_POSITION_LOOP_P, float >,
    cfg_reg< CONTROL_POSITION_LOOP_I, float >,
    cfg_reg< CONTROL_POSITION_LOOP_D, float >,
    cfg_reg< CONTROL_POSITION_LIM_MIN, float >,
    cfg_reg< CONTROL_POSITION_LIM_MAX, float >,
    cfg_reg< MINIMUM_VOLTAGE, float >,
    cfg_reg< MAXIMUM_TEMPERATURE, float > >;

using cfg_value_message = typename cfg_map::message_type;

struct cfg_keyval
{
        cfg_key           key;
        cfg_value_message msg;
};

consteval cfg_map get_default_config()
{
        return cfg_map{
            cfg_reg< REVERSED, bool >{ false },
            cfg_reg< POSITION_CONV_LOWER_SETPOINT_VALUE, uint16_t >{ 0 },
            cfg_reg< POSITION_CONV_LOWER_SETPOINT_ANGLE, float >{ 0.f },
            cfg_reg< POSITION_CONV_HIGHER_SETPOINT_VALUE, uint16_t >{ 4096 },
            cfg_reg< POSITION_CONV_HIGHER_SETPOINT_ANGLE, float >{ 2 * pi },
            cfg_reg< CURRENT_CONV_SCALE, float >{ 1.f },
            cfg_reg< CURRENT_CONV_OFFSET, float >{ 0.f },
            cfg_reg< TEMP_CONV_SCALE, float >{ 1.f },
            cfg_reg< TEMP_CONV_OFFSET, float >{ 0.f },
            cfg_reg< VOLTAGE_CONV_SCALE, float >{ 1.f },
            cfg_reg< CONTROL_CURRENT_LOOP_P, float >{ 1024.f * 32.f },
            cfg_reg< CONTROL_CURRENT_LOOP_I, float >{ 4.f },
            cfg_reg< CONTROL_CURRENT_LOOP_D, float >{ 0.f },
            cfg_reg< CONTROL_CURRENT_LIM_MIN, float >{ -3.f },
            cfg_reg< CONTROL_CURRENT_LIM_MAX, float >{ 3.f },
            cfg_reg< CONTROL_VELOCITY_LOOP_P, float >{ 0.01f },
            cfg_reg< CONTROL_VELOCITY_LOOP_I, float >{ 0.0000001f },
            cfg_reg< CONTROL_VELOCITY_LOOP_D, float >{ 0.f },
            cfg_reg< CONTROL_VELOCITY_LIM_MIN, float >{ -3.f },
            cfg_reg< CONTROL_VELOCITY_LIM_MAX, float >{ 3.f },
            cfg_reg< CONTROL_POSITION_LOOP_P, float >{ 1.f },
            cfg_reg< CONTROL_POSITION_LOOP_I, float >{ 0.f },
            cfg_reg< CONTROL_POSITION_LOOP_D, float >{ 0.f },
            cfg_reg< CONTROL_POSITION_LIM_MIN, float >{ -10.f },
            cfg_reg< CONTROL_POSITION_LIM_MAX, float >{ 10.f },
            cfg_reg< MINIMUM_VOLTAGE, float >{ 6.f },
            cfg_reg< MAXIMUM_TEMPERATURE, float >{ 80.f },
        };
}
