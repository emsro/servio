#include "base/base.hpp"
#include "cfg/key.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <emlabcpp/experimental/testing/convert.h>
#include <emlabcpp/protocol/register_handler.h>
#include <emlabcpp/protocol/register_map.h>
#include <magic_enum.hpp>

#pragma once

namespace em = emlabcpp;

namespace servio::cfg
{
struct payload
{
        em::string_buffer< 16 > git_ver;
        em::string_buffer< 32 > git_date;
        uint32_t                id;

        friend constexpr auto operator<=>( const payload&, const payload& ) = default;
};

template < auto Key, typename T >
using reg = em::protocol::register_pair< Key, T >;

using model_name = em::string_buffer< 32 >;

enum encoder_mode : uint8_t
{
        ENC_MODE_ANALOG = 0,
        ENC_MODE_QUAD   = 1
};

using map = em::protocol::register_map<
    std::endian::little,
    reg< MODEL, model_name >,
    reg< ID, uint32_t >,
    reg< GROUP_ID, uint32_t >,
    reg< ENCODER_MODE, encoder_mode >,
    reg< POSITION_LOW_ANGLE, float >,
    reg< POSITION_HIGH_ANGLE, float >,
    reg< CURRENT_CONV_SCALE, float >,
    reg< CURRENT_CONV_OFFSET, float >,
    reg< TEMP_CONV_SCALE, float >,
    reg< TEMP_CONV_OFFSET, float >,
    reg< VOLTAGE_CONV_SCALE, float >,
    reg< INVERT_HBRIDGE, bool >,
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
    reg< VELOCITY_TO_CURR_LIM_SCALE, float >,
    reg< POSITION_LOOP_P, float >,
    reg< POSITION_LOOP_I, float >,
    reg< POSITION_LOOP_D, float >,
    reg< POSITION_LIM_MIN, float >,
    reg< POSITION_LIM_MAX, float >,
    reg< POSITION_TO_VEL_LIM_SCALE, float >,
    reg< STATIC_FRICTION_SCALE, float >,
    reg< STATIC_FRICTION_DECAY, float >,
    reg< MINIMUM_VOLTAGE, float >,
    reg< MAXIMUM_TEMPERATURE, float >,
    reg< MOVING_DETECTION_STEP, float >,
    reg< QUAD_ENCD_RANGE, uint32_t > >;

using value_message = typename map::message_type;
using value_variant = std::variant< model_name, uint32_t, float, bool, encoder_mode >;

struct keyval
{
        cfg::key      key;
        value_message msg;
};

template < key Key >
keyval make_keyval( auto val )
{
        return {
            .key = Key,
            .msg = em::protocol::register_handler< map >::serialize< Key >( val ),
        };
}

struct context
{
        cfg::payload payload;
        cfg::map     map;
};

// TODO: find all inlines and minimize
#ifdef EMLABCPP_USE_OSTREAM
inline std::ostream& operator<<( std::ostream& os, const encoder_mode& m )
{
        return os << magic_enum::enum_name( m );
}
#endif

}  // namespace servio::cfg

namespace emlabcpp::testing
{
template <>
struct value_type_converter< servio::cfg::encoder_mode >
  : value_type_converter_enum< servio::cfg::encoder_mode >
{
};
}  // namespace emlabcpp::testing
