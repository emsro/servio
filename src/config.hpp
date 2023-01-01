
#include <emlabcpp/protocol/register_map.h>

#pragma once

namespace em = emlabcpp;

enum class cfg_key : uint16_t
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
};

template < auto Key, typename T >
using cfg_reg = em::protocol::register_pair< Key, T >;

// TODO: this overlaps /o\ write a check for reg_map that confirms that config values does not
// overlap
using cfg_map = em::protocol::register_map<
    std::endian::little,
    cfg_reg< cfg_key::REVERSED, bool >,
    cfg_reg< cfg_key::POSITION_CONV_LOWER_SETPOINT_VALUE, uint16_t >,
    cfg_reg< cfg_key::POSITION_CONV_LOWER_SETPOINT_ANGLE, float >,
    cfg_reg< cfg_key::POSITION_CONV_HIGHER_SETPOINT_VALUE, uint16_t >,
    cfg_reg< cfg_key::POSITION_CONV_HIGHER_SETPOINT_ANGLE, float >,
    cfg_reg< cfg_key::CURRENT_CONV_SCALE, float >,
    cfg_reg< cfg_key::CURRENT_CONV_OFFSET, float >,
    cfg_reg< cfg_key::TEMP_CONV_SCALE, float >,
    cfg_reg< cfg_key::TEMP_CONV_OFFSET, float >,
    cfg_reg< cfg_key::VOLTAGE_CONV_SCALE, float > >;

using cfg_value_message = typename cfg_map::message_type;

struct cfg_keyval
{
    cfg_key           key;
    cfg_value_message msg;
};
