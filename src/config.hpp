#include "base.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <emlabcpp/protocol/register_map.h>

#pragma once

namespace em = emlabcpp;

enum cfg_key : uint32_t
{
        MODEL                               = 1,
        ID                                  = 2,
        POSITION_CONV_LOWER_SETPOINT_VALUE  = 10,
        POSITION_CONV_LOWER_SETPOINT_ANGLE  = 11,
        POSITION_CONV_HIGHER_SETPOINT_VALUE = 12,
        POSITION_CONV_HIGHER_SETPOINT_ANGLE = 13,
        CURRENT_CONV_SCALE                  = 14,
        CURRENT_CONV_OFFSET                 = 15,
        TEMP_CONV_SCALE                     = 16,
        TEMP_CONV_OFFSET                    = 17,
        VOLTAGE_CONV_SCALE                  = 18,
        CURRENT_LOOP_P                      = 30,
        CURRENT_LOOP_I                      = 31,
        CURRENT_LOOP_D                      = 32,
        CURRENT_LIM_MIN                     = 33,
        CURRENT_LIM_MAX                     = 34,
        VELOCITY_LOOP_P                     = 40,
        VELOCITY_LOOP_I                     = 41,
        VELOCITY_LOOP_D                     = 42,
        VELOCITY_LIM_MIN                    = 43,
        VELOCITY_LIM_MAX                    = 44,
        POSITION_LOOP_P                     = 50,
        POSITION_LOOP_I                     = 51,
        POSITION_LOOP_D                     = 52,
        POSITION_LIM_MIN                    = 53,
        POSITION_LIM_MAX                    = 54,
        STATIC_FRICTION_SCALE               = 60,
        STATIC_FRICTION_DECAY               = 61,
        MINIMUM_VOLTAGE                     = 62,
        MAXIMUM_TEMPERATURE                 = 65,
        MOVING_DETECTION_STEP               = 66,
};

template < auto Key, typename T >
using cfg_reg = em::protocol::register_pair< Key, T >;

using model_name = em::string_buffer< 32 >;

// TODO: write a check for reg_map that confirms that config values does not
// overlap
using cfg_map = em::protocol::register_map<
    std::endian::little,
    cfg_reg< MODEL, model_name >,
    cfg_reg< ID, uint32_t >,
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
