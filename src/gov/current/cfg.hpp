#pragma once

#include "../../base.hpp"

namespace servio::gov::curr::cfg
{
using value_type = vari::typelist< uint32_t, float, bool >;
// GEN BEGIN HERE
enum class key : uint32_t
{
        loop_p                = 1,
        loop_i                = 2,
        loop_d                = 3,
        curr_lim_min          = 4,
        curr_lim_max          = 5,
        pos_lim_min           = 6,
        pos_lim_max           = 7,
        pos_to_curr_lim_scale = 8,
};

constexpr std::string_view to_str( key k )
{
        if ( k == key::loop_p )
                return "loop_p";
        if ( k == key::loop_i )
                return "loop_i";
        if ( k == key::loop_d )
                return "loop_d";
        if ( k == key::curr_lim_min )
                return "curr_lim_min";
        if ( k == key::curr_lim_max )
                return "curr_lim_max";
        if ( k == key::pos_lim_min )
                return "pos_lim_min";
        if ( k == key::pos_lim_max )
                return "pos_lim_max";
        if ( k == key::pos_to_curr_lim_scale )
                return "pos_to_curr_lim_scale";
        return {};
}

constexpr uint32_t key_to_id( key k )
{
        if ( k == key::loop_p )
                return 1;
        if ( k == key::loop_i )
                return 2;
        if ( k == key::loop_d )
                return 3;
        if ( k == key::curr_lim_min )
                return 4;
        if ( k == key::curr_lim_max )
                return 5;
        if ( k == key::pos_lim_min )
                return 6;
        if ( k == key::pos_lim_max )
                return 7;
        if ( k == key::pos_to_curr_lim_scale )
                return 8;
        return {};
}

constexpr key id_to_key( uint32_t id )
{
        if ( id == 1 )
                return key::loop_p;
        if ( id == 2 )
                return key::loop_i;
        if ( id == 3 )
                return key::loop_d;
        if ( id == 4 )
                return key::curr_lim_min;
        if ( id == 5 )
                return key::curr_lim_max;
        if ( id == 6 )
                return key::pos_lim_min;
        if ( id == 7 )
                return key::pos_lim_max;
        if ( id == 8 )
                return key::pos_to_curr_lim_scale;
        return {};
}

constexpr std::array< uint32_t, 8 > ids = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
};
template < key K >
struct key_trait;

template <>
struct key_trait< key::loop_p >
{
        using type = float;
};

template <>
struct key_trait< key::loop_i >
{
        using type = float;
};

template <>
struct key_trait< key::loop_d >
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

struct map
{
        using key_type                             = key;
        static constexpr std::array< key, 8 > keys = {
            key::loop_p,
            key::loop_i,
            key::loop_d,
            key::curr_lim_min,
            key::curr_lim_max,
            key::pos_lim_min,
            key::pos_lim_max,
            key::pos_to_curr_lim_scale,
        };
        float loop_p = 1.0F;
        float loop_i = 0.0001F;
        float loop_d = 0.0F;
        // Soft limit for lowest current
        float curr_lim_min = -2.0F;
        // Soft limit for highest current
        float curr_lim_max = 2.0F;
        // Soft limit for lowest position
        float pos_lim_min = -2.0F;
        // Soft limit for highest position
        float pos_lim_max = 2.0F;
        // Scale factor for position to current limit conversion
        float pos_to_curr_lim_scale = 0.1F;

        template < key K >
        constexpr auto& ref_by_key()
        {
                if constexpr ( K == key::loop_p )
                        return loop_p;
                if constexpr ( K == key::loop_i )
                        return loop_i;
                if constexpr ( K == key::loop_d )
                        return loop_d;
                if constexpr ( K == key::curr_lim_min )
                        return curr_lim_min;
                if constexpr ( K == key::curr_lim_max )
                        return curr_lim_max;
                if constexpr ( K == key::pos_lim_min )
                        return pos_lim_min;
                if constexpr ( K == key::pos_lim_max )
                        return pos_lim_max;
                if constexpr ( K == key::pos_to_curr_lim_scale )
                        return pos_to_curr_lim_scale;
        }

        constexpr vari::vptr< value_type > ref_by_key( key K )
        {
                switch ( K ) {
                case key::loop_p:
                        return &loop_p;
                case key::loop_i:
                        return &loop_i;
                case key::loop_d:
                        return &loop_d;
                case key::curr_lim_min:
                        return &curr_lim_min;
                case key::curr_lim_max:
                        return &curr_lim_max;
                case key::pos_lim_min:
                        return &pos_lim_min;
                case key::pos_lim_max:
                        return &pos_lim_max;
                case key::pos_to_curr_lim_scale:
                        return &pos_to_curr_lim_scale;
                }
                return nullptr;
        }

        constexpr vari::vptr< value_type const > ref_by_key( key K ) const
        {
                switch ( K ) {
                case key::loop_p:
                        return &loop_p;
                case key::loop_i:
                        return &loop_i;
                case key::loop_d:
                        return &loop_d;
                case key::curr_lim_min:
                        return &curr_lim_min;
                case key::curr_lim_max:
                        return &curr_lim_max;
                case key::pos_lim_min:
                        return &pos_lim_min;
                case key::pos_lim_max:
                        return &pos_lim_max;
                case key::pos_to_curr_lim_scale:
                        return &pos_to_curr_lim_scale;
                }
                return nullptr;
        }

        constexpr vari::vptr< value_type > ref_by_id( uint32_t id )
        {
                if ( id == 1 )
                        return &loop_p;
                if ( id == 2 )
                        return &loop_i;
                if ( id == 3 )
                        return &loop_d;
                if ( id == 4 )
                        return &curr_lim_min;
                if ( id == 5 )
                        return &curr_lim_max;
                if ( id == 6 )
                        return &pos_lim_min;
                if ( id == 7 )
                        return &pos_lim_max;
                if ( id == 8 )
                        return &pos_to_curr_lim_scale;
                return {};
        }

        constexpr vari::vptr< value_type const > ref_by_id( uint32_t id ) const
        {
                if ( id == 1 )
                        return &loop_p;
                if ( id == 2 )
                        return &loop_i;
                if ( id == 3 )
                        return &loop_d;
                if ( id == 4 )
                        return &curr_lim_min;
                if ( id == 5 )
                        return &curr_lim_max;
                if ( id == 6 )
                        return &pos_lim_min;
                if ( id == 7 )
                        return &pos_lim_max;
                if ( id == 8 )
                        return &pos_to_curr_lim_scale;
                return {};
        }

        static constexpr std::optional< key > str_to_key( std::string_view str )
        {
                if ( str == "loop_p" )
                        return key::loop_p;
                if ( str == "loop_i" )
                        return key::loop_i;
                if ( str == "loop_d" )
                        return key::loop_d;
                if ( str == "curr_lim_min" )
                        return key::curr_lim_min;
                if ( str == "curr_lim_max" )
                        return key::curr_lim_max;
                if ( str == "pos_lim_min" )
                        return key::pos_lim_min;
                if ( str == "pos_lim_max" )
                        return key::pos_lim_max;
                if ( str == "pos_to_curr_lim_scale" )
                        return key::pos_to_curr_lim_scale;
                return {};
        }
};

// GEN END HERE
}  // namespace servio::gov::curr::cfg
