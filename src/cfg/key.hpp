#pragma once

#include <cstdint>

namespace cfg
{

enum key : uint32_t
{
        MODEL                               = 1,
        ID                                  = 2,
        GROUP_ID                            = 3,
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

}
