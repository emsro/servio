#pragma once

#include "../base.hpp"

#include <emlabcpp/experimental/string_buffer.h>
#include <vari/vref.h>

namespace servio::cfg
{

namespace em = emlabcpp;

using string_type = em::string_buffer< 32 >;
enum class encoder_mode : uint8_t
{
        analog = 1,
        quad   = 2,
};

constexpr std::optional< encoder_mode > str_to_encoder_mode( std::string_view str )
{
        if ( str == "analog" )
                return encoder_mode::analog;
        if ( str == "quad" )
                return encoder_mode::quad;
        return {};
}

using value_type = vari::typelist< uint32_t, float, bool, encoder_mode, string_type >;

// GEN BEGIN HERE
// This part of file is auto-generated. Do not edit.
enum class key : uint32_t
{
        model                          = 1,
        id                             = 2,
        group_id                       = 3,
        encoder_mode                   = 4,
        position_low_angle             = 11,
        position_high_angle            = 12,
        current_conv_scale             = 14,
        current_conv_offset            = 15,
        temp_conv_scale                = 16,
        temp_conv_offset               = 17,
        voltage_conv_scale             = 18,
        invert_hbridge                 = 19,
        current_loop_p                 = 30,
        current_loop_i                 = 31,
        current_loop_d                 = 32,
        current_lim_min                = 33,
        current_lim_max                = 34,
        velocity_loop_p                = 40,
        velocity_loop_i                = 41,
        velocity_loop_d                = 42,
        velocity_lim_min               = 43,
        velocity_lim_max               = 44,
        velocity_to_current_lim_scale  = 45,
        position_loop_p                = 50,
        position_loop_i                = 51,
        position_loop_d                = 52,
        position_lim_min               = 53,
        position_lim_max               = 54,
        position_to_velocity_lim_scale = 55,
        static_friction_scale          = 60,
        static_friction_decay          = 61,
        minimum_voltage                = 62,
        maximum_temperature            = 65,
        moving_detection_step          = 66,
        quad_encoder_range             = 80,
};
constexpr std::array< key, 35 > keys = {
    key::model,
    key::id,
    key::group_id,
    key::encoder_mode,
    key::position_low_angle,
    key::position_high_angle,
    key::current_conv_scale,
    key::current_conv_offset,
    key::temp_conv_scale,
    key::temp_conv_offset,
    key::voltage_conv_scale,
    key::invert_hbridge,
    key::current_loop_p,
    key::current_loop_i,
    key::current_loop_d,
    key::current_lim_min,
    key::current_lim_max,
    key::velocity_loop_p,
    key::velocity_loop_i,
    key::velocity_loop_d,
    key::velocity_lim_min,
    key::velocity_lim_max,
    key::velocity_to_current_lim_scale,
    key::position_loop_p,
    key::position_loop_i,
    key::position_loop_d,
    key::position_lim_min,
    key::position_lim_max,
    key::position_to_velocity_lim_scale,
    key::static_friction_scale,
    key::static_friction_decay,
    key::minimum_voltage,
    key::maximum_temperature,
    key::moving_detection_step,
    key::quad_encoder_range,
};

constexpr std::optional< key > str_to_key( std::string_view str )
{
        if ( str == "model" )
                return key::model;
        if ( str == "id" )
                return key::id;
        if ( str == "group_id" )
                return key::group_id;
        if ( str == "encoder_mode" )
                return key::encoder_mode;
        if ( str == "position_low_angle" )
                return key::position_low_angle;
        if ( str == "position_high_angle" )
                return key::position_high_angle;
        if ( str == "current_conv_scale" )
                return key::current_conv_scale;
        if ( str == "current_conv_offset" )
                return key::current_conv_offset;
        if ( str == "temp_conv_scale" )
                return key::temp_conv_scale;
        if ( str == "temp_conv_offset" )
                return key::temp_conv_offset;
        if ( str == "voltage_conv_scale" )
                return key::voltage_conv_scale;
        if ( str == "invert_hbridge" )
                return key::invert_hbridge;
        if ( str == "current_loop_p" )
                return key::current_loop_p;
        if ( str == "current_loop_i" )
                return key::current_loop_i;
        if ( str == "current_loop_d" )
                return key::current_loop_d;
        if ( str == "current_lim_min" )
                return key::current_lim_min;
        if ( str == "current_lim_max" )
                return key::current_lim_max;
        if ( str == "velocity_loop_p" )
                return key::velocity_loop_p;
        if ( str == "velocity_loop_i" )
                return key::velocity_loop_i;
        if ( str == "velocity_loop_d" )
                return key::velocity_loop_d;
        if ( str == "velocity_lim_min" )
                return key::velocity_lim_min;
        if ( str == "velocity_lim_max" )
                return key::velocity_lim_max;
        if ( str == "velocity_to_current_lim_scale" )
                return key::velocity_to_current_lim_scale;
        if ( str == "position_loop_p" )
                return key::position_loop_p;
        if ( str == "position_loop_i" )
                return key::position_loop_i;
        if ( str == "position_loop_d" )
                return key::position_loop_d;
        if ( str == "position_lim_min" )
                return key::position_lim_min;
        if ( str == "position_lim_max" )
                return key::position_lim_max;
        if ( str == "position_to_velocity_lim_scale" )
                return key::position_to_velocity_lim_scale;
        if ( str == "static_friction_scale" )
                return key::static_friction_scale;
        if ( str == "static_friction_decay" )
                return key::static_friction_decay;
        if ( str == "minimum_voltage" )
                return key::minimum_voltage;
        if ( str == "maximum_temperature" )
                return key::maximum_temperature;
        if ( str == "moving_detection_step" )
                return key::moving_detection_step;
        if ( str == "quad_encoder_range" )
                return key::quad_encoder_range;
        return {};
}

constexpr std::array< uint32_t, 35 > ids = {
    1,  2,  3,  4,  11, 12, 14, 15, 16, 17, 18, 19, 30, 31, 32, 33, 34, 40,
    41, 42, 43, 44, 45, 50, 51, 52, 53, 54, 55, 60, 61, 62, 65, 66, 80,
};

struct map
{
        // Model of the servomotor, used for debugging purposes
        string_type model = "no model";
        // ID of the servomotor
        uint32_t id = 0;
        // Group ID of the servomotor
        uint32_t group_id = 0;
        // Encoder mode of the servomotor
        cfg::encoder_mode encoder_mode                   = cfg::encoder_mode::analog;
        float             position_low_angle             = 0.0;
        float             position_high_angle            = 2 * pi;
        float             current_conv_scale             = 1.0;
        float             current_conv_offset            = 0.0;
        float             temp_conv_scale                = 1.0;
        float             temp_conv_offset               = 0.0;
        float             voltage_conv_scale             = 1.0;
        bool              invert_hbridge                 = false;
        float             current_loop_p                 = 1.0;
        float             current_loop_i                 = 0.0001F;
        float             current_loop_d                 = 0.0;
        float             current_lim_min                = -2.0;
        float             current_lim_max                = 2.0;
        float             velocity_loop_p                = 1.0;
        float             velocity_loop_i                = 0.000'000'2F;
        float             velocity_loop_d                = 0.0;
        float             velocity_lim_min               = -3.0;
        float             velocity_lim_max               = 3.0;
        float             velocity_to_current_lim_scale  = 2.0;
        float             position_loop_p                = 0.2F;
        float             position_loop_i                = 0.000'000'02F;
        float             position_loop_d                = 0.0;
        float             position_lim_min               = 0.1F;
        float             position_lim_max               = 2.0F * pi - 0.1F;
        float             position_to_velocity_lim_scale = 2.0;
        float             static_friction_scale          = 3.0;
        float             static_friction_decay          = 1.0;
        float             minimum_voltage                = 6.0;
        float             maximum_temperature            = 80.0;
        float             moving_detection_step          = 0.05F;
        uint32_t          quad_encoder_range             = 2048;

        template < key K >
        constexpr auto& ref_by_key()
        {
                if constexpr ( K == key::model )
                        return model;
                if constexpr ( K == key::id )
                        return id;
                if constexpr ( K == key::group_id )
                        return group_id;
                if constexpr ( K == key::encoder_mode )
                        return encoder_mode;
                if constexpr ( K == key::position_low_angle )
                        return position_low_angle;
                if constexpr ( K == key::position_high_angle )
                        return position_high_angle;
                if constexpr ( K == key::current_conv_scale )
                        return current_conv_scale;
                if constexpr ( K == key::current_conv_offset )
                        return current_conv_offset;
                if constexpr ( K == key::temp_conv_scale )
                        return temp_conv_scale;
                if constexpr ( K == key::temp_conv_offset )
                        return temp_conv_offset;
                if constexpr ( K == key::voltage_conv_scale )
                        return voltage_conv_scale;
                if constexpr ( K == key::invert_hbridge )
                        return invert_hbridge;
                if constexpr ( K == key::current_loop_p )
                        return current_loop_p;
                if constexpr ( K == key::current_loop_i )
                        return current_loop_i;
                if constexpr ( K == key::current_loop_d )
                        return current_loop_d;
                if constexpr ( K == key::current_lim_min )
                        return current_lim_min;
                if constexpr ( K == key::current_lim_max )
                        return current_lim_max;
                if constexpr ( K == key::velocity_loop_p )
                        return velocity_loop_p;
                if constexpr ( K == key::velocity_loop_i )
                        return velocity_loop_i;
                if constexpr ( K == key::velocity_loop_d )
                        return velocity_loop_d;
                if constexpr ( K == key::velocity_lim_min )
                        return velocity_lim_min;
                if constexpr ( K == key::velocity_lim_max )
                        return velocity_lim_max;
                if constexpr ( K == key::velocity_to_current_lim_scale )
                        return velocity_to_current_lim_scale;
                if constexpr ( K == key::position_loop_p )
                        return position_loop_p;
                if constexpr ( K == key::position_loop_i )
                        return position_loop_i;
                if constexpr ( K == key::position_loop_d )
                        return position_loop_d;
                if constexpr ( K == key::position_lim_min )
                        return position_lim_min;
                if constexpr ( K == key::position_lim_max )
                        return position_lim_max;
                if constexpr ( K == key::position_to_velocity_lim_scale )
                        return position_to_velocity_lim_scale;
                if constexpr ( K == key::static_friction_scale )
                        return static_friction_scale;
                if constexpr ( K == key::static_friction_decay )
                        return static_friction_decay;
                if constexpr ( K == key::minimum_voltage )
                        return minimum_voltage;
                if constexpr ( K == key::maximum_temperature )
                        return maximum_temperature;
                if constexpr ( K == key::moving_detection_step )
                        return moving_detection_step;
                if constexpr ( K == key::quad_encoder_range )
                        return quad_encoder_range;
        }

        constexpr vari::vptr< value_type > ref_by_id( uint32_t id )
        {
                if ( id == 1 )
                        return &model;
                if ( id == 2 )
                        return &id;
                if ( id == 3 )
                        return &group_id;
                if ( id == 4 )
                        return &encoder_mode;
                if ( id == 11 )
                        return &position_low_angle;
                if ( id == 12 )
                        return &position_high_angle;
                if ( id == 14 )
                        return &current_conv_scale;
                if ( id == 15 )
                        return &current_conv_offset;
                if ( id == 16 )
                        return &temp_conv_scale;
                if ( id == 17 )
                        return &temp_conv_offset;
                if ( id == 18 )
                        return &voltage_conv_scale;
                if ( id == 19 )
                        return &invert_hbridge;
                if ( id == 30 )
                        return &current_loop_p;
                if ( id == 31 )
                        return &current_loop_i;
                if ( id == 32 )
                        return &current_loop_d;
                if ( id == 33 )
                        return &current_lim_min;
                if ( id == 34 )
                        return &current_lim_max;
                if ( id == 40 )
                        return &velocity_loop_p;
                if ( id == 41 )
                        return &velocity_loop_i;
                if ( id == 42 )
                        return &velocity_loop_d;
                if ( id == 43 )
                        return &velocity_lim_min;
                if ( id == 44 )
                        return &velocity_lim_max;
                if ( id == 45 )
                        return &velocity_to_current_lim_scale;
                if ( id == 50 )
                        return &position_loop_p;
                if ( id == 51 )
                        return &position_loop_i;
                if ( id == 52 )
                        return &position_loop_d;
                if ( id == 53 )
                        return &position_lim_min;
                if ( id == 54 )
                        return &position_lim_max;
                if ( id == 55 )
                        return &position_to_velocity_lim_scale;
                if ( id == 60 )
                        return &static_friction_scale;
                if ( id == 61 )
                        return &static_friction_decay;
                if ( id == 62 )
                        return &minimum_voltage;
                if ( id == 65 )
                        return &maximum_temperature;
                if ( id == 66 )
                        return &moving_detection_step;
                if ( id == 80 )
                        return &quad_encoder_range;
                return {};
        }

        constexpr vari::vptr< value_type const > ref_by_id( uint32_t id ) const
        {
                if ( id == 1 )
                        return &model;
                if ( id == 2 )
                        return &id;
                if ( id == 3 )
                        return &group_id;
                if ( id == 4 )
                        return &encoder_mode;
                if ( id == 11 )
                        return &position_low_angle;
                if ( id == 12 )
                        return &position_high_angle;
                if ( id == 14 )
                        return &current_conv_scale;
                if ( id == 15 )
                        return &current_conv_offset;
                if ( id == 16 )
                        return &temp_conv_scale;
                if ( id == 17 )
                        return &temp_conv_offset;
                if ( id == 18 )
                        return &voltage_conv_scale;
                if ( id == 19 )
                        return &invert_hbridge;
                if ( id == 30 )
                        return &current_loop_p;
                if ( id == 31 )
                        return &current_loop_i;
                if ( id == 32 )
                        return &current_loop_d;
                if ( id == 33 )
                        return &current_lim_min;
                if ( id == 34 )
                        return &current_lim_max;
                if ( id == 40 )
                        return &velocity_loop_p;
                if ( id == 41 )
                        return &velocity_loop_i;
                if ( id == 42 )
                        return &velocity_loop_d;
                if ( id == 43 )
                        return &velocity_lim_min;
                if ( id == 44 )
                        return &velocity_lim_max;
                if ( id == 45 )
                        return &velocity_to_current_lim_scale;
                if ( id == 50 )
                        return &position_loop_p;
                if ( id == 51 )
                        return &position_loop_i;
                if ( id == 52 )
                        return &position_loop_d;
                if ( id == 53 )
                        return &position_lim_min;
                if ( id == 54 )
                        return &position_lim_max;
                if ( id == 55 )
                        return &position_to_velocity_lim_scale;
                if ( id == 60 )
                        return &static_friction_scale;
                if ( id == 61 )
                        return &static_friction_decay;
                if ( id == 62 )
                        return &minimum_voltage;
                if ( id == 65 )
                        return &maximum_temperature;
                if ( id == 66 )
                        return &moving_detection_step;
                if ( id == 80 )
                        return &quad_encoder_range;
                return {};
        }
};

// GEN END HERE

struct context
{
        cfg::map map;
};

}  // namespace servio::cfg
