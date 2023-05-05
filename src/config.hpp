#include "base.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <emlabcpp/protocol/register_map.h>

#pragma once

namespace em = emlabcpp;

enum cfg_key : uint16_t
{
        MODEL                               = 0x01,
        POSITION_CONV_LOWER_SETPOINT_VALUE  = 0x02,
        POSITION_CONV_LOWER_SETPOINT_ANGLE  = 0x03,
        POSITION_CONV_HIGHER_SETPOINT_VALUE = 0x04,
        POSITION_CONV_HIGHER_SETPOINT_ANGLE = 0x05,
        CURRENT_CONV_SCALE                  = 0x06,
        CURRENT_CONV_OFFSET                 = 0x07,
        TEMP_CONV_SCALE                     = 0x08,
        TEMP_CONV_OFFSET                    = 0x09,
        VOLTAGE_CONV_SCALE                  = 0x0a,
        CONTROL_CURRENT_LOOP_P              = 0x0b,
        CONTROL_CURRENT_LOOP_I              = 0x0c,
        CONTROL_CURRENT_LOOP_D              = 0x0d,
        CONTROL_CURRENT_LIM_MIN             = 0x0e,
        CONTROL_CURRENT_LIM_MAX             = 0x0f,
        CONTROL_VELOCITY_LOOP_P             = 0x10,
        CONTROL_VELOCITY_LOOP_I             = 0x11,
        CONTROL_VELOCITY_LOOP_D             = 0x12,
        CONTROL_VELOCITY_LIM_MIN            = 0x13,
        CONTROL_VELOCITY_LIM_MAX            = 0x14,
        CONTROL_POSITION_LOOP_P             = 0x15,
        CONTROL_POSITION_LOOP_I             = 0x16,
        CONTROL_POSITION_LOOP_D             = 0x17,
        CONTROL_POSITION_LIM_MIN            = 0x18,
        CONTROL_POSITION_LIM_MAX            = 0x19,
        CONTROL_STATIC_FRICTION_SCALE       = 0x1a,
        CONTROL_STATIC_FRICTION_DECAY       = 0x1b,
        MINIMUM_VOLTAGE                     = 0x1c,
        MAXIMUM_TEMPERATURE                 = 0x1e,
        MOVING_DETECTION_STEP               = 0x1f,
};

template < auto Key, typename T >
using cfg_reg = em::protocol::register_pair< Key, T >;

using model_name = em::string_buffer< 32 >;

// TODO: write a check for reg_map that confirms that config values does not
// overlap
using cfg_map = em::protocol::register_map<
    std::endian::little,
    cfg_reg< MODEL, model_name >,
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
    cfg_reg< CONTROL_STATIC_FRICTION_SCALE, float >,
    cfg_reg< CONTROL_STATIC_FRICTION_DECAY, float >,
    cfg_reg< MINIMUM_VOLTAGE, float >,
    cfg_reg< MAXIMUM_TEMPERATURE, float >,
    cfg_reg< MOVING_DETECTION_STEP, float > >;

using cfg_value_message = typename cfg_map::message_type;

struct cfg_keyval
{
        cfg_key           key;
        cfg_value_message msg;
};
