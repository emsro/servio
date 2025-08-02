#pragma once

#include "../../base.hpp"

namespace servio::gov::vel::cfg
{
using value_type = vari::typelist< uint32_t, float, bool >;

// GEN BEGIN HERE
enum class key : uint32_t
{
        loop_p                = 1,
        loop_i                = 2,
        loop_d                = 3,
        static_friction_scale = 4,
        static_friction_decay = 5,
};

constexpr std::string_view to_str( key k )
{
        if ( k == key::loop_p )
                return "loop_p";
        if ( k == key::loop_i )
                return "loop_i";
        if ( k == key::loop_d )
                return "loop_d";
        if ( k == key::static_friction_scale )
                return "static_friction_scale";
        if ( k == key::static_friction_decay )
                return "static_friction_decay";
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
        if ( k == key::static_friction_scale )
                return 4;
        if ( k == key::static_friction_decay )
                return 5;
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
                return key::static_friction_scale;
        if ( id == 5 )
                return key::static_friction_decay;
        return {};
}

constexpr std::array< uint32_t, 5 > ids = {
    1,
    2,
    3,
    4,
    5,
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
struct key_trait< key::static_friction_scale >
{
        using type = float;
};

template <>
struct key_trait< key::static_friction_decay >
{
        using type = float;
};

struct map
{
        using key_type                             = key;
        static constexpr std::array< key, 5 > keys = {
            key::loop_p,
            key::loop_i,
            key::loop_d,
            key::static_friction_scale,
            key::static_friction_decay,
        };
        float loop_p = 1.0F;
        float loop_i = 0.0001F;
        float loop_d = 0.0F;
        // Scaling of current in case of non-moving servo
        float static_friction_scale = 3.0;
        // Time required for the static friction compensation to decay to be ineffective
        float static_friction_decay = 1.0;

        template < key K >
        constexpr auto& ref_by_key()
        {
                if constexpr ( K == key::loop_p )
                        return loop_p;
                if constexpr ( K == key::loop_i )
                        return loop_i;
                if constexpr ( K == key::loop_d )
                        return loop_d;
                if constexpr ( K == key::static_friction_scale )
                        return static_friction_scale;
                if constexpr ( K == key::static_friction_decay )
                        return static_friction_decay;
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
                case key::static_friction_scale:
                        return &static_friction_scale;
                case key::static_friction_decay:
                        return &static_friction_decay;
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
                case key::static_friction_scale:
                        return &static_friction_scale;
                case key::static_friction_decay:
                        return &static_friction_decay;
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
                        return &static_friction_scale;
                if ( id == 5 )
                        return &static_friction_decay;
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
                        return &static_friction_scale;
                if ( id == 5 )
                        return &static_friction_decay;
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
                if ( str == "static_friction_scale" )
                        return key::static_friction_scale;
                if ( str == "static_friction_decay" )
                        return key::static_friction_decay;
                return {};
        }
};

// GEN END HERE
}  // namespace servio::gov::vel::cfg
