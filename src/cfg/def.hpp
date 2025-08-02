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

using value_type = vari::typelist< uint32_t, float, bool, encoder_mode, string >;

// GEN BEGIN HERE
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
        if ( k == key::current_loop_p )
                return "current_loop_p";
        if ( k == key::current_loop_i )
                return "current_loop_i";
        if ( k == key::current_loop_d )
                return "current_loop_d";
        if ( k == key::current_lim_min )
                return "current_lim_min";
        if ( k == key::current_lim_max )
                return "current_lim_max";
        if ( k == key::velocity_loop_p )
                return "velocity_loop_p";
        if ( k == key::velocity_loop_i )
                return "velocity_loop_i";
        if ( k == key::velocity_loop_d )
                return "velocity_loop_d";
        if ( k == key::velocity_lim_min )
                return "velocity_lim_min";
        if ( k == key::velocity_lim_max )
                return "velocity_lim_max";
        if ( k == key::velocity_to_current_lim_scale )
                return "velocity_to_current_lim_scale";
        if ( k == key::position_loop_p )
                return "position_loop_p";
        if ( k == key::position_loop_i )
                return "position_loop_i";
        if ( k == key::position_loop_d )
                return "position_loop_d";
        if ( k == key::position_lim_min )
                return "position_lim_min";
        if ( k == key::position_lim_max )
                return "position_lim_max";
        if ( k == key::position_to_velocity_lim_scale )
                return "position_to_velocity_lim_scale";
        if ( k == key::static_friction_scale )
                return "static_friction_scale";
        if ( k == key::static_friction_decay )
                return "static_friction_decay";
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
        if ( k == key::current_loop_p )
                return 30;
        if ( k == key::current_loop_i )
                return 31;
        if ( k == key::current_loop_d )
                return 32;
        if ( k == key::current_lim_min )
                return 33;
        if ( k == key::current_lim_max )
                return 34;
        if ( k == key::velocity_loop_p )
                return 40;
        if ( k == key::velocity_loop_i )
                return 41;
        if ( k == key::velocity_loop_d )
                return 42;
        if ( k == key::velocity_lim_min )
                return 43;
        if ( k == key::velocity_lim_max )
                return 44;
        if ( k == key::velocity_to_current_lim_scale )
                return 45;
        if ( k == key::position_loop_p )
                return 50;
        if ( k == key::position_loop_i )
                return 51;
        if ( k == key::position_loop_d )
                return 52;
        if ( k == key::position_lim_min )
                return 53;
        if ( k == key::position_lim_max )
                return 54;
        if ( k == key::position_to_velocity_lim_scale )
                return 55;
        if ( k == key::static_friction_scale )
                return 60;
        if ( k == key::static_friction_decay )
                return 61;
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
        if ( id == 30 )
                return key::current_loop_p;
        if ( id == 31 )
                return key::current_loop_i;
        if ( id == 32 )
                return key::current_loop_d;
        if ( id == 33 )
                return key::current_lim_min;
        if ( id == 34 )
                return key::current_lim_max;
        if ( id == 40 )
                return key::velocity_loop_p;
        if ( id == 41 )
                return key::velocity_loop_i;
        if ( id == 42 )
                return key::velocity_loop_d;
        if ( id == 43 )
                return key::velocity_lim_min;
        if ( id == 44 )
                return key::velocity_lim_max;
        if ( id == 45 )
                return key::velocity_to_current_lim_scale;
        if ( id == 50 )
                return key::position_loop_p;
        if ( id == 51 )
                return key::position_loop_i;
        if ( id == 52 )
                return key::position_loop_d;
        if ( id == 53 )
                return key::position_lim_min;
        if ( id == 54 )
                return key::position_lim_max;
        if ( id == 55 )
                return key::position_to_velocity_lim_scale;
        if ( id == 60 )
                return key::static_friction_scale;
        if ( id == 61 )
                return key::static_friction_decay;
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

constexpr std::array< uint32_t, 35 > ids = {
    1,  2,  3,  4,  11, 12, 14, 15, 16, 17, 18, 19, 30, 31, 32, 33, 34, 40,
    41, 42, 43, 44, 45, 50, 51, 52, 53, 54, 55, 60, 61, 62, 65, 66, 80,
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
struct key_trait< key::current_loop_p >
{
        using type = float;
};

template <>
struct key_trait< key::current_loop_i >
{
        using type = float;
};

template <>
struct key_trait< key::current_loop_d >
{
        using type = float;
};

template <>
struct key_trait< key::current_lim_min >
{
        using type = float;
};

template <>
struct key_trait< key::current_lim_max >
{
        using type = float;
};

template <>
struct key_trait< key::velocity_loop_p >
{
        using type = float;
};

template <>
struct key_trait< key::velocity_loop_i >
{
        using type = float;
};

template <>
struct key_trait< key::velocity_loop_d >
{
        using type = float;
};

template <>
struct key_trait< key::velocity_lim_min >
{
        using type = float;
};

template <>
struct key_trait< key::velocity_lim_max >
{
        using type = float;
};

template <>
struct key_trait< key::velocity_to_current_lim_scale >
{
        using type = float;
};

template <>
struct key_trait< key::position_loop_p >
{
        using type = float;
};

template <>
struct key_trait< key::position_loop_i >
{
        using type = float;
};

template <>
struct key_trait< key::position_loop_d >
{
        using type = float;
};

template <>
struct key_trait< key::position_lim_min >
{
        using type = float;
};

template <>
struct key_trait< key::position_lim_max >
{
        using type = float;
};

template <>
struct key_trait< key::position_to_velocity_lim_scale >
{
        using type = float;
};

template <>
struct key_trait< key::static_friction_scale >
{
        using type = float;
};

template <>
struct key_trait< key::static_friction_decay >
{
        using type = float;
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
        static constexpr std::array< key, 35 > keys = {
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
        bool  invert_hbridge = false;
        float current_loop_p = 1.0;
        float current_loop_i = 0.0001F;
        float current_loop_d = 0.0;
        // Soft limit for lowest current
        float current_lim_min = -2.0;
        // Soft limit for highest current
        float current_lim_max = 2.0;
        float velocity_loop_p = 1.0;
        float velocity_loop_i = 0.000'000'2F;
        float velocity_loop_d = 0.0;
        // Soft limit for lowest velocity
        float velocity_lim_min = -3.0;
        // Soft limit for highest velocity
        float velocity_lim_max = 3.0;
        // Scaling of current limit derived from velocity limit
        float velocity_to_current_lim_scale = 2.0;
        float position_loop_p               = 0.2F;
        float position_loop_i               = 0.000'000'02F;
        float position_loop_d               = 0.0;
        float position_lim_min              = 0.1F;
        float position_lim_max              = 2.0F * pi - 0.1F;
        // Scaling of velocity limit derived from position limit
        float position_to_velocity_lim_scale = 2.0;
        // Scaling of current in case of non-moving servo
        float static_friction_scale = 3.0;
        // Time required for the static friction compensation to decay to be ineffective
        float static_friction_decay = 1.0;
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
                case key::current_loop_p:
                        return &current_loop_p;
                case key::current_loop_i:
                        return &current_loop_i;
                case key::current_loop_d:
                        return &current_loop_d;
                case key::current_lim_min:
                        return &current_lim_min;
                case key::current_lim_max:
                        return &current_lim_max;
                case key::velocity_loop_p:
                        return &velocity_loop_p;
                case key::velocity_loop_i:
                        return &velocity_loop_i;
                case key::velocity_loop_d:
                        return &velocity_loop_d;
                case key::velocity_lim_min:
                        return &velocity_lim_min;
                case key::velocity_lim_max:
                        return &velocity_lim_max;
                case key::velocity_to_current_lim_scale:
                        return &velocity_to_current_lim_scale;
                case key::position_loop_p:
                        return &position_loop_p;
                case key::position_loop_i:
                        return &position_loop_i;
                case key::position_loop_d:
                        return &position_loop_d;
                case key::position_lim_min:
                        return &position_lim_min;
                case key::position_lim_max:
                        return &position_lim_max;
                case key::position_to_velocity_lim_scale:
                        return &position_to_velocity_lim_scale;
                case key::static_friction_scale:
                        return &static_friction_scale;
                case key::static_friction_decay:
                        return &static_friction_decay;
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
                case key::current_loop_p:
                        return &current_loop_p;
                case key::current_loop_i:
                        return &current_loop_i;
                case key::current_loop_d:
                        return &current_loop_d;
                case key::current_lim_min:
                        return &current_lim_min;
                case key::current_lim_max:
                        return &current_lim_max;
                case key::velocity_loop_p:
                        return &velocity_loop_p;
                case key::velocity_loop_i:
                        return &velocity_loop_i;
                case key::velocity_loop_d:
                        return &velocity_loop_d;
                case key::velocity_lim_min:
                        return &velocity_lim_min;
                case key::velocity_lim_max:
                        return &velocity_lim_max;
                case key::velocity_to_current_lim_scale:
                        return &velocity_to_current_lim_scale;
                case key::position_loop_p:
                        return &position_loop_p;
                case key::position_loop_i:
                        return &position_loop_i;
                case key::position_loop_d:
                        return &position_loop_d;
                case key::position_lim_min:
                        return &position_lim_min;
                case key::position_lim_max:
                        return &position_lim_max;
                case key::position_to_velocity_lim_scale:
                        return &position_to_velocity_lim_scale;
                case key::static_friction_scale:
                        return &static_friction_scale;
                case key::static_friction_decay:
                        return &static_friction_decay;
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
};

// GEN END HERE

struct context
{
        cfg::map map;
};

}  // namespace servio::cfg
