#include "scaled.hpp"

#include <chrono>
#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <numbers>

#pragma once

namespace em = emlabcpp;

using protocol_power       = int16_t;
using protocol_current     = scaled< int32_t, 16 >;
using protocol_velocity    = scaled< int32_t, 16 >;
using protocol_position    = scaled< int32_t, 16 >;
using protocol_voltage     = scaled< int32_t, 16 >;
using protocol_temperature = scaled< int32_t, 16 >;

using microseconds = std::chrono::duration< uint32_t, std::micro >;

constexpr microseconds operator""_s( unsigned long long int secs )
{
        return microseconds{ static_cast< uint32_t >( secs * 1'000'000u ) };
}

constexpr microseconds operator""_ms( unsigned long long int msecs )
{
        return microseconds{ static_cast< uint32_t >( msecs * 1'000u ) };
}

constexpr microseconds operator""_us( unsigned long long int usecs )
{
        return microseconds{ static_cast< uint32_t >( usecs ) };
}

using sec_time = std::chrono::duration< float >;

template < typename T >
using limits = em::min_max< T >;

static constexpr float infty = std::numeric_limits< float >::infinity();
static constexpr float pi    = std::numbers::pi_v< float >;

enum class control_mode
{
        POWER,
        CURRENT,
        VELOCITY,
        POSITION
};

enum class control_loop
{
        CURRENT,
        VELOCITY,
        POSITION
};

struct leds_vals
{
        bool    red;
        bool    blue;
        uint8_t yellow;
        uint8_t green;
};
