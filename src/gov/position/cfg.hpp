#pragma once

#include "../../base.hpp"

namespace servio::gov::pos::cfg
{
using value_type = vari::typelist< uint32_t, float, bool >;

// GEN BEGIN HERE
enum class key : uint32_t
{
        pos_loop_p            = 1,
        pos_loop_i            = 2,
        pos_loop_d            = 3,
        curr_loop_p           = 4,
        curr_loop_i           = 5,
        curr_loop_d           = 6,
        static_friction_scale = 7,
        static_friction_decay = 8,
        pos_lim_min           = 9,
        pos_lim_max           = 10,
        pos_to_curr_lim_scale = 11,
        curr_lim_min          = 12,
        curr_lim_max          = 13,
};

constexpr std::string_view to_str( key k )
{
        if ( k == key::pos_loop_p )
                return "pos_loop_p";
        if ( k == key::pos_loop_i )
                return "pos_loop_i";
        if ( k == key::pos_loop_d )
                return "pos_loop_d";
        if ( k == key::curr_loop_p )
                return "curr_loop_p";
        if ( k == key::curr_loop_i )
                return "curr_loop_i";
        if ( k == key::curr_loop_d )
                return "curr_loop_d";
        if ( k == key::static_friction_scale )
                return "static_friction_scale";
        if ( k == key::static_friction_decay )
                return "static_friction_decay";
        if ( k == key::pos_lim_min )
                return "pos_lim_min";
        if ( k == key::pos_lim_max )
                return "pos_lim_max";
        if ( k == key::pos_to_curr_lim_scale )
                return "pos_to_curr_lim_scale";
        if ( k == key::curr_lim_min )
                return "curr_lim_min";
        if ( k == key::curr_lim_max )
                return "curr_lim_max";
        return {};
}

constexpr uint32_t key_to_id( key k )
{
        if ( k == key::pos_loop_p )
                return 1;
        if ( k == key::pos_loop_i )
                return 2;
        if ( k == key::pos_loop_d )
                return 3;
        if ( k == key::curr_loop_p )
                return 4;
        if ( k == key::curr_loop_i )
                return 5;
        if ( k == key::curr_loop_d )
                return 6;
        if ( k == key::static_friction_scale )
                return 7;
        if ( k == key::static_friction_decay )
                return 8;
        if ( k == key::pos_lim_min )
                return 9;
        if ( k == key::pos_lim_max )
                return 10;
        if ( k == key::pos_to_curr_lim_scale )
                return 11;
        if ( k == key::curr_lim_min )
                return 12;
        if ( k == key::curr_lim_max )
                return 13;
        return {};
}

constexpr key id_to_key( uint32_t id )
{
        if ( id == 1 )
                return key::pos_loop_p;
        if ( id == 2 )
                return key::pos_loop_i;
        if ( id == 3 )
                return key::pos_loop_d;
        if ( id == 4 )
                return key::curr_loop_p;
        if ( id == 5 )
                return key::curr_loop_i;
        if ( id == 6 )
                return key::curr_loop_d;
        if ( id == 7 )
                return key::static_friction_scale;
        if ( id == 8 )
                return key::static_friction_decay;
        if ( id == 9 )
                return key::pos_lim_min;
        if ( id == 10 )
                return key::pos_lim_max;
        if ( id == 11 )
                return key::pos_to_curr_lim_scale;
        if ( id == 12 )
                return key::curr_lim_min;
        if ( id == 13 )
                return key::curr_lim_max;
        return {};
}

constexpr std::array< uint32_t, 13 > ids = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
};
template < key K >
struct key_trait;

template <>
struct key_trait< key::pos_loop_p >
{
        using type = float;
};

template <>
struct key_trait< key::pos_loop_i >
{
        using type = float;
};

template <>
struct key_trait< key::pos_loop_d >
{
        using type = float;
};

template <>
struct key_trait< key::curr_loop_p >
{
        using type = float;
};

template <>
struct key_trait< key::curr_loop_i >
{
        using type = float;
};

template <>
struct key_trait< key::curr_loop_d >
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
struct key_trait< key::pos_lim_min >
{
        using type = float;
};

template <>
struct key_trait< key::pos_lim_max >
{
        using type = float;
};

template <>
struct key_trait< key::pos_to_curr_lim_scale >
{
        using type = float;
};

template <>
struct key_trait< key::curr_lim_min >
{
        using type = float;
};

template <>
struct key_trait< key::curr_lim_max >
{
        using type = float;
};

struct map
{
        using key_type                              = key;
        static constexpr std::array< key, 13 > keys = {
            key::pos_loop_p,
            key::pos_loop_i,
            key::pos_loop_d,
            key::curr_loop_p,
            key::curr_loop_i,
            key::curr_loop_d,
            key::static_friction_scale,
            key::static_friction_decay,
            key::pos_lim_min,
            key::pos_lim_max,
            key::pos_to_curr_lim_scale,
            key::curr_lim_min,
            key::curr_lim_max,
        };
        float pos_loop_p  = 1.0F;
        float pos_loop_i  = 0.0001F;
        float pos_loop_d  = 0.0F;
        float curr_loop_p = 1.0F;
        float curr_loop_i = 0.0001F;
        float curr_loop_d = 0.0F;
        // Scaling of current in case of non-moving servo
        float static_friction_scale = 3.0;
        // Time required for the static friction compensation to decay to be ineffective
        float static_friction_decay = 1.0;
        // Soft limit for lowest position
        float pos_lim_min = -2.0F;
        // Soft limit for highest position
        float pos_lim_max = 2.0F;
        // Scale factor for position to current limit conversion
        float pos_to_curr_lim_scale = 0.1F;
        // Soft limit for lowest current
        float curr_lim_min = -2.0F;
        // Soft limit for highest current
        float curr_lim_max = 2.0F;

        template < key K >
        constexpr auto& ref_by_key()
        {
                if constexpr ( K == key::pos_loop_p )
                        return pos_loop_p;
                if constexpr ( K == key::pos_loop_i )
                        return pos_loop_i;
                if constexpr ( K == key::pos_loop_d )
                        return pos_loop_d;
                if constexpr ( K == key::curr_loop_p )
                        return curr_loop_p;
                if constexpr ( K == key::curr_loop_i )
                        return curr_loop_i;
                if constexpr ( K == key::curr_loop_d )
                        return curr_loop_d;
                if constexpr ( K == key::static_friction_scale )
                        return static_friction_scale;
                if constexpr ( K == key::static_friction_decay )
                        return static_friction_decay;
                if constexpr ( K == key::pos_lim_min )
                        return pos_lim_min;
                if constexpr ( K == key::pos_lim_max )
                        return pos_lim_max;
                if constexpr ( K == key::pos_to_curr_lim_scale )
                        return pos_to_curr_lim_scale;
                if constexpr ( K == key::curr_lim_min )
                        return curr_lim_min;
                if constexpr ( K == key::curr_lim_max )
                        return curr_lim_max;
        }

        constexpr vari::vptr< value_type > ref_by_key( key K )
        {
                switch ( K ) {
                case key::pos_loop_p:
                        return &pos_loop_p;
                case key::pos_loop_i:
                        return &pos_loop_i;
                case key::pos_loop_d:
                        return &pos_loop_d;
                case key::curr_loop_p:
                        return &curr_loop_p;
                case key::curr_loop_i:
                        return &curr_loop_i;
                case key::curr_loop_d:
                        return &curr_loop_d;
                case key::static_friction_scale:
                        return &static_friction_scale;
                case key::static_friction_decay:
                        return &static_friction_decay;
                case key::pos_lim_min:
                        return &pos_lim_min;
                case key::pos_lim_max:
                        return &pos_lim_max;
                case key::pos_to_curr_lim_scale:
                        return &pos_to_curr_lim_scale;
                case key::curr_lim_min:
                        return &curr_lim_min;
                case key::curr_lim_max:
                        return &curr_lim_max;
                }
                return nullptr;
        }

        constexpr vari::vptr< value_type const > ref_by_key( key K ) const
        {
                switch ( K ) {
                case key::pos_loop_p:
                        return &pos_loop_p;
                case key::pos_loop_i:
                        return &pos_loop_i;
                case key::pos_loop_d:
                        return &pos_loop_d;
                case key::curr_loop_p:
                        return &curr_loop_p;
                case key::curr_loop_i:
                        return &curr_loop_i;
                case key::curr_loop_d:
                        return &curr_loop_d;
                case key::static_friction_scale:
                        return &static_friction_scale;
                case key::static_friction_decay:
                        return &static_friction_decay;
                case key::pos_lim_min:
                        return &pos_lim_min;
                case key::pos_lim_max:
                        return &pos_lim_max;
                case key::pos_to_curr_lim_scale:
                        return &pos_to_curr_lim_scale;
                case key::curr_lim_min:
                        return &curr_lim_min;
                case key::curr_lim_max:
                        return &curr_lim_max;
                }
                return nullptr;
        }

        constexpr vari::vptr< value_type > ref_by_id( uint32_t id )
        {
                if ( id == 1 )
                        return &pos_loop_p;
                if ( id == 2 )
                        return &pos_loop_i;
                if ( id == 3 )
                        return &pos_loop_d;
                if ( id == 4 )
                        return &curr_loop_p;
                if ( id == 5 )
                        return &curr_loop_i;
                if ( id == 6 )
                        return &curr_loop_d;
                if ( id == 7 )
                        return &static_friction_scale;
                if ( id == 8 )
                        return &static_friction_decay;
                if ( id == 9 )
                        return &pos_lim_min;
                if ( id == 10 )
                        return &pos_lim_max;
                if ( id == 11 )
                        return &pos_to_curr_lim_scale;
                if ( id == 12 )
                        return &curr_lim_min;
                if ( id == 13 )
                        return &curr_lim_max;
                return {};
        }

        constexpr vari::vptr< value_type const > ref_by_id( uint32_t id ) const
        {
                if ( id == 1 )
                        return &pos_loop_p;
                if ( id == 2 )
                        return &pos_loop_i;
                if ( id == 3 )
                        return &pos_loop_d;
                if ( id == 4 )
                        return &curr_loop_p;
                if ( id == 5 )
                        return &curr_loop_i;
                if ( id == 6 )
                        return &curr_loop_d;
                if ( id == 7 )
                        return &static_friction_scale;
                if ( id == 8 )
                        return &static_friction_decay;
                if ( id == 9 )
                        return &pos_lim_min;
                if ( id == 10 )
                        return &pos_lim_max;
                if ( id == 11 )
                        return &pos_to_curr_lim_scale;
                if ( id == 12 )
                        return &curr_lim_min;
                if ( id == 13 )
                        return &curr_lim_max;
                return {};
        }

        static constexpr std::optional< key > str_to_key( std::string_view str )
        {
                if ( str == "pos_loop_p" )
                        return key::pos_loop_p;
                if ( str == "pos_loop_i" )
                        return key::pos_loop_i;
                if ( str == "pos_loop_d" )
                        return key::pos_loop_d;
                if ( str == "curr_loop_p" )
                        return key::curr_loop_p;
                if ( str == "curr_loop_i" )
                        return key::curr_loop_i;
                if ( str == "curr_loop_d" )
                        return key::curr_loop_d;
                if ( str == "static_friction_scale" )
                        return key::static_friction_scale;
                if ( str == "static_friction_decay" )
                        return key::static_friction_decay;
                if ( str == "pos_lim_min" )
                        return key::pos_lim_min;
                if ( str == "pos_lim_max" )
                        return key::pos_lim_max;
                if ( str == "pos_to_curr_lim_scale" )
                        return key::pos_to_curr_lim_scale;
                if ( str == "curr_lim_min" )
                        return key::curr_lim_min;
                if ( str == "curr_lim_max" )
                        return key::curr_lim_max;
                return {};
        }
};

// GEN END HERE
}  // namespace servio::gov::pos::cfg
