#pragma once

#include <array>
#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <initializer_list>
#include <type_traits>

namespace em = emlabcpp;

namespace servio::base
{

// TODO: analyze status system whenever it is finished and ready to use
enum class [[nodiscard]] status : uint8_t
{
        UNINITIALIZED = 0,
        NOMINAL       = 1,
        DEGRADED      = 2,
        INOPERABLE    = 3,
};

inline void escalate_status( status& s, const status n )
{
        using T   = std::underlying_type_t< status >;
        T max_val = std::max( static_cast< T >( s ), static_cast< T >( n ) );
        s         = static_cast< status >( max_val );
}

inline status worst_of( std::initializer_list< status > statuses )
{
        auto worst = em::min_elem( statuses, [&]( status s ) {
                return static_cast< std::underlying_type_t< status > >( s );
        } );
        return static_cast< status >( worst );
}

}  // namespace servio::base
