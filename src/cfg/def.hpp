#pragma once

#include "./base.hpp"

namespace servio::cfg
{

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

constexpr std::string_view encoder_mode_to_str( encoder_mode em )
{
        switch ( em ) {
        case encoder_mode::analog:
                return "analog";
        case encoder_mode::quad:
                return "quad";
        }
        return "invalid";
}

template <>
struct cfg_type_trait< encoder_mode >
{
        using base_type = string;

        static opt_str_err load( encoder_mode& tg, base_type& inpt )
        {
                auto em = str_to_encoder_mode( inpt );
                if ( !em )
                        return "unknown encoder mode"_err;
                tg = *em;
                return {};
        }

        static string store( encoder_mode const& val )
        {
                return encoder_mode_to_str( val );
        }
};

using value_type    = vari::typelist< uint32_t, float, bool, encoder_mode, string >;
using value_variant = std::variant< uint32_t, float, bool, encoder_mode, string >;

// GEN BEGIN HERE
enum class key : uint32_t
{
        model                 = 1,
        id                    = 2,
        group_id              = 3,
        encoder_mode          = 4,
        position_low_angle    = 11,
        position_high_angle   = 12,
        current_conv_scale    = 14,
        current_conv_offset   = 15,
        temp_conv_scale       = 16,
        temp_conv_offset      = 17,
        voltage_conv_scale    = 18,
        invert_hbridge        = 19,
        minimum_voltage       = 62,
        maximum_temperature   = 65,
        moving_detection_step = 66,
        quad_encoder_range    = 80,
};

constexpr std::string_view to_str( key k )
{
        if ( k == key::model )
                return "model";
        if ( k == key::id )
                return "id";
        if ( k == key::group_id )
                return "group_id";
        if ( k == key::encoder_mode )
                return "encoder_mode";
        if ( k == key::position_low_angle )
                return "position_low_angle";
        if ( k == key::position_high_angle )
                return "position_high_angle";
        if ( k == key::current_conv_scale )
                return "current_conv_scale";
        if ( k == key::current_conv_offset )
                return "current_conv_offset";
        if ( k == key::temp_conv_scale )
                return "temp_conv_scale";
        if ( k == key::temp_conv_offset )
                return "temp_conv_offset";
        if ( k == key::voltage_conv_scale )
                return "voltage_conv_scale";
        if ( k == key::invert_hbridge )
                return "invert_hbridge";
        if ( k == key::minimum_voltage )
                return "minimum_voltage";
        if ( k == key::maximum_temperature )
                return "maximum_temperature";
        if ( k == key::moving_detection_step )
                return "moving_detection_step";
        if ( k == key::quad_encoder_range )
                return "quad_encoder_range";
        return {};
}

constexpr uint32_t key_to_id( key k )
{
        if ( k == key::model )
                return 1;
        if ( k == key::id )
                return 2;
        if ( k == key::group_id )
                return 3;
        if ( k == key::encoder_mode )
                return 4;
        if ( k == key::position_low_angle )
                return 11;
        if ( k == key::position_high_angle )
                return 12;
        if ( k == key::current_conv_scale )
                return 14;
        if ( k == key::current_conv_offset )
                return 15;
        if ( k == key::temp_conv_scale )
                return 16;
        if ( k == key::temp_conv_offset )
                return 17;
        if ( k == key::voltage_conv_scale )
                return 18;
        if ( k == key::invert_hbridge )
                return 19;
        if ( k == key::minimum_voltage )
                return 62;
        if ( k == key::maximum_temperature )
                return 65;
        if ( k == key::moving_detection_step )
                return 66;
        if ( k == key::quad_encoder_range )
                return 80;
        return {};
}

constexpr key id_to_key( uint32_t id )
{
        if ( id == 1 )
                return key::model;
        if ( id == 2 )
                return key::id;
        if ( id == 3 )
                return key::group_id;
        if ( id == 4 )
                return key::encoder_mode;
        if ( id == 11 )
                return key::position_low_angle;
        if ( id == 12 )
                return key::position_high_angle;
        if ( id == 14 )
                return key::current_conv_scale;
        if ( id == 15 )
                return key::current_conv_offset;
        if ( id == 16 )
                return key::temp_conv_scale;
        if ( id == 17 )
                return key::temp_conv_offset;
        if ( id == 18 )
                return key::voltage_conv_scale;
        if ( id == 19 )
                return key::invert_hbridge;
        if ( id == 62 )
                return key::minimum_voltage;
        if ( id == 65 )
                return key::maximum_temperature;
        if ( id == 66 )
                return key::moving_detection_step;
        if ( id == 80 )
                return key::quad_encoder_range;
        return {};
}

constexpr std::array< uint32_t, 16 > ids = {
    1,
    2,
    3,
    4,
    11,
    12,
    14,
    15,
    16,
    17,
    18,
    19,
    62,
    65,
    66,
    80,
};
template < key K >
struct key_trait;

template <>
struct key_trait< key::model >
{
        using type = string;
};

template <>
struct key_trait< key::id >
{
        using type = uint32_t;
};

template <>
struct key_trait< key::group_id >
{
        using type = uint32_t;
};

template <>
struct key_trait< key::encoder_mode >
{
        using type = cfg::encoder_mode;
};

template <>
struct key_trait< key::position_low_angle >
{
        using type = float;
};

template <>
struct key_trait< key::position_high_angle >
{
        using type = float;
};

template <>
struct key_trait< key::current_conv_scale >
{
        using type = float;
};

template <>
struct key_trait< key::current_conv_offset >
{
        using type = float;
};

template <>
struct key_trait< key::temp_conv_scale >
{
        using type = float;
};

template <>
struct key_trait< key::temp_conv_offset >
{
        using type = float;
};

template <>
struct key_trait< key::voltage_conv_scale >
{
        using type = float;
};

template <>
struct key_trait< key::invert_hbridge >
{
        using type = bool;
};

template <>
struct key_trait< key::minimum_voltage >
{
        using type = float;
};

template <>
struct key_trait< key::maximum_temperature >
{
        using type = float;
};

template <>
struct key_trait< key::moving_detection_step >
{
        using type = float;
};

template <>
struct key_trait< key::quad_encoder_range >
{
        using type = uint32_t;
};

struct map
{
        using key_type                              = key;
        static constexpr std::array< key, 16 > keys = {
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
            key::minimum_voltage,
            key::maximum_temperature,
            key::moving_detection_step,
            key::quad_encoder_range,
        };
        // Model of the servomotor, used for debugging purposes
        string model = "no model";
        // ID of the servomotor
        uint32_t id = 0;
        // Group ID of the servomotor
        uint32_t group_id = 0;
        // Encoder mode of the servomotor
        cfg::encoder_mode encoder_mode = cfg::encoder_mode::analog;
        // Expected lowest position of the servomotor
        float position_low_angle = 0.0;
        // Expected highest position of the servomotor
        float position_high_angle = 2 * pi;
        // Current conversion scale factor for formula: C = ADC * scale + offset
        float current_conv_scale = 1.0;
        // Current conversion offset for formula: C = ADC * scale + offset
        float current_conv_offset = 0.0;
        // Temperature conversion scale factor for formula: T = ADC * scale + offset
        float temp_conv_scale = 1.0;
        // Temperature conversion offset for formula: T = ADC * scale + offset
        float temp_conv_offset = 0.0;
        // Voltage conversion scale factor for formula: V = ADC * scale
        float voltage_conv_scale = 1.0;
        // Invert direction of the H-bridge
        bool invert_hbridge = false;
        // Minimum voltage for the servomotor to operate
        float minimum_voltage = 6.0;
        // Maximum temperature for the servomotor to operate
        float maximum_temperature = 80.0;
        // Deadband for the moving detection
        float moving_detection_step = 0.05F;
        // Number of pulses per revolution of the quadrature encoder
        uint32_t quad_encoder_range = 2048;

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
                if constexpr ( K == key::minimum_voltage )
                        return minimum_voltage;
                if constexpr ( K == key::maximum_temperature )
                        return maximum_temperature;
                if constexpr ( K == key::moving_detection_step )
                        return moving_detection_step;
                if constexpr ( K == key::quad_encoder_range )
                        return quad_encoder_range;
        }

        constexpr vari::vptr< value_type > ref_by_key( key K )
        {
                switch ( K ) {
                case key::model:
                        return &model;
                case key::id:
                        return &id;
                case key::group_id:
                        return &group_id;
                case key::encoder_mode:
                        return &encoder_mode;
                case key::position_low_angle:
                        return &position_low_angle;
                case key::position_high_angle:
                        return &position_high_angle;
                case key::current_conv_scale:
                        return &current_conv_scale;
                case key::current_conv_offset:
                        return &current_conv_offset;
                case key::temp_conv_scale:
                        return &temp_conv_scale;
                case key::temp_conv_offset:
                        return &temp_conv_offset;
                case key::voltage_conv_scale:
                        return &voltage_conv_scale;
                case key::invert_hbridge:
                        return &invert_hbridge;
                case key::minimum_voltage:
                        return &minimum_voltage;
                case key::maximum_temperature:
                        return &maximum_temperature;
                case key::moving_detection_step:
                        return &moving_detection_step;
                case key::quad_encoder_range:
                        return &quad_encoder_range;
                }
                return nullptr;
        }

        constexpr vari::vptr< value_type const > ref_by_key( key K ) const
        {
                switch ( K ) {
                case key::model:
                        return &model;
                case key::id:
                        return &id;
                case key::group_id:
                        return &group_id;
                case key::encoder_mode:
                        return &encoder_mode;
                case key::position_low_angle:
                        return &position_low_angle;
                case key::position_high_angle:
                        return &position_high_angle;
                case key::current_conv_scale:
                        return &current_conv_scale;
                case key::current_conv_offset:
                        return &current_conv_offset;
                case key::temp_conv_scale:
                        return &temp_conv_scale;
                case key::temp_conv_offset:
                        return &temp_conv_offset;
                case key::voltage_conv_scale:
                        return &voltage_conv_scale;
                case key::invert_hbridge:
                        return &invert_hbridge;
                case key::minimum_voltage:
                        return &minimum_voltage;
                case key::maximum_temperature:
                        return &maximum_temperature;
                case key::moving_detection_step:
                        return &moving_detection_step;
                case key::quad_encoder_range:
                        return &quad_encoder_range;
                }
                return nullptr;
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

        static constexpr std::optional< key > str_to_key( std::string_view str )
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
};

// GEN END HERE

struct context
{
        cfg::map map;
};

}  // namespace servio::cfg
