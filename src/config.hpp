#include "base.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <emlabcpp/protocol/register_map.h>

#pragma once

namespace em = emlabcpp;

enum cfg_key : uint32_t
{
        MODEL                               = 1,
        POSITION_CONV_LOWER_SETPOINT_VALUE  = 2,
        POSITION_CONV_LOWER_SETPOINT_ANGLE  = 3,
        POSITION_CONV_HIGHER_SETPOINT_VALUE = 4,
        POSITION_CONV_HIGHER_SETPOINT_ANGLE = 5,
        CURRENT_CONV_SCALE                  = 6,
        CURRENT_CONV_OFFSET                 = 7,
        TEMP_CONV_SCALE                     = 8,
        TEMP_CONV_OFFSET                    = 9,
        VOLTAGE_CONV_SCALE                  = 10,
        CURRENT_LOOP_P                      = 11,
        CURRENT_LOOP_I                      = 12,
        CURRENT_LOOP_D                      = 13,
        CURRENT_LIM_MIN                     = 14,
        CURRENT_LIM_MAX                     = 15,
        VELOCITY_LOOP_P                     = 16,
        VELOCITY_LOOP_I                     = 17,
        VELOCITY_LOOP_D                     = 18,
        VELOCITY_LIM_MIN                    = 19,
        VELOCITY_LIM_MAX                    = 20,
        POSITION_LOOP_P                     = 21,
        POSITION_LOOP_I                     = 22,
        POSITION_LOOP_D                     = 23,
        POSITION_LIM_MIN                    = 24,
        POSITION_LIM_MAX                    = 25,
        STATIC_FRICTION_SCALE               = 26,
        STATIC_FRICTION_DECAY               = 27,
        MINIMUM_VOLTAGE                     = 28,
        MAXIMUM_TEMPERATURE                 = 29,
        MOVING_DETECTION_STEP               = 30,
};

template < auto Key, typename T >
using cfg_reg = em::protocol::register_pair< Key, T >;

using model_name = em::string_buffer< 32 >;

// TODO: write a check for reg_map that confirms that config values does not
// overlap
using cfg_map = em::protocol::register_map<
    std::endian::little,
    cfg_reg< MODEL, model_name >,
    cfg_reg< POSITION_CONV_LOWER_SETPOINT_VALUE, uint32_t >,
    cfg_reg< POSITION_CONV_LOWER_SETPOINT_ANGLE, float >,
    cfg_reg< POSITION_CONV_HIGHER_SETPOINT_VALUE, uint32_t >,
    cfg_reg< POSITION_CONV_HIGHER_SETPOINT_ANGLE, float >,
    cfg_reg< CURRENT_CONV_SCALE, float >,
    cfg_reg< CURRENT_CONV_OFFSET, float >,
    cfg_reg< TEMP_CONV_SCALE, float >,
    cfg_reg< TEMP_CONV_OFFSET, float >,
    cfg_reg< VOLTAGE_CONV_SCALE, float >,
    cfg_reg< CURRENT_LOOP_P, float >,
    cfg_reg< CURRENT_LOOP_I, float >,
    cfg_reg< CURRENT_LOOP_D, float >,
    cfg_reg< CURRENT_LIM_MIN, float >,
    cfg_reg< CURRENT_LIM_MAX, float >,
    cfg_reg< VELOCITY_LOOP_P, float >,
    cfg_reg< VELOCITY_LOOP_I, float >,
    cfg_reg< VELOCITY_LOOP_D, float >,
    cfg_reg< VELOCITY_LIM_MIN, float >,
    cfg_reg< VELOCITY_LIM_MAX, float >,
    cfg_reg< POSITION_LOOP_P, float >,
    cfg_reg< POSITION_LOOP_I, float >,
    cfg_reg< POSITION_LOOP_D, float >,
    cfg_reg< POSITION_LIM_MIN, float >,
    cfg_reg< POSITION_LIM_MAX, float >,
    cfg_reg< STATIC_FRICTION_SCALE, float >,
    cfg_reg< STATIC_FRICTION_DECAY, float >,
    cfg_reg< MINIMUM_VOLTAGE, float >,
    cfg_reg< MAXIMUM_TEMPERATURE, float >,
    cfg_reg< MOVING_DETECTION_STEP, float > >;

using cfg_value_message = typename cfg_map::message_type;

struct cfg_keyval
{
        cfg_key           key;
        cfg_value_message msg;
};
