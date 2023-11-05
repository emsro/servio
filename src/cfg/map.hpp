#include "base.hpp"
#include "cfg/key.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <emlabcpp/protocol/register_handler.h>
#include <emlabcpp/protocol/register_map.h>

#pragma once

namespace em = emlabcpp;

namespace cfg
{

template < auto Key, typename T >
using reg = em::protocol::register_pair< Key, T >;

using model_name = em::string_buffer< 32 >;

// TODO: write a check for reg_map that confirms that config values does not
// overlap
using map = em::protocol::register_map<
    std::endian::little,
    reg< MODEL, model_name >,
    reg< ID, uint32_t >,
    reg< GROUP_ID, uint32_t >,
    reg< POSITION_CONV_LOWER_SETPOINT_VALUE, uint32_t >,
    reg< POSITION_CONV_LOWER_SETPOINT_ANGLE, float >,
    reg< POSITION_CONV_HIGHER_SETPOINT_VALUE, uint32_t >,
    reg< POSITION_CONV_HIGHER_SETPOINT_ANGLE, float >,
    reg< CURRENT_CONV_SCALE, float >,
    reg< CURRENT_CONV_OFFSET, float >,
    reg< TEMP_CONV_SCALE, float >,
    reg< TEMP_CONV_OFFSET, float >,
    reg< VOLTAGE_CONV_SCALE, float >,
    reg< CURRENT_LOOP_P, float >,
    reg< CURRENT_LOOP_I, float >,
    reg< CURRENT_LOOP_D, float >,
    reg< CURRENT_LIM_MIN, float >,
    reg< CURRENT_LIM_MAX, float >,
    reg< VELOCITY_LOOP_P, float >,
    reg< VELOCITY_LOOP_I, float >,
    reg< VELOCITY_LOOP_D, float >,
    reg< VELOCITY_LIM_MIN, float >,
    reg< VELOCITY_LIM_MAX, float >,
    reg< POSITION_LOOP_P, float >,
    reg< POSITION_LOOP_I, float >,
    reg< POSITION_LOOP_D, float >,
    reg< POSITION_LIM_MIN, float >,
    reg< POSITION_LIM_MAX, float >,
    reg< STATIC_FRICTION_SCALE, float >,
    reg< STATIC_FRICTION_DECAY, float >,
    reg< MINIMUM_VOLTAGE, float >,
    reg< MAXIMUM_TEMPERATURE, float >,
    reg< MOVING_DETECTION_STEP, float > >;

using value_message = typename map::message_type;

struct keyval
{
        cfg::key      key;
        value_message msg;
};

template < key key >
keyval make_keyval( auto val )
{
        return {
            .key = key,
            .msg = em::protocol::register_handler< map >::serialize< key >( val ),
        };
}

}  // namespace cfg
