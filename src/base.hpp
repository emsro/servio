#include "scaled.hpp"

#include <chrono>
#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <numbers>

#pragma once

namespace servio
{

namespace em = emlabcpp;

using microseconds = std::chrono::duration< uint32_t, std::micro >;
using milliseconds = std::chrono::duration< uint32_t, std::milli >;
using seconds      = std::chrono::duration< uint32_t >;

constexpr microseconds operator""_s( unsigned long long int secs )
{
        return microseconds{ static_cast< uint32_t >( secs * 1'000'000U ) };
}

constexpr microseconds operator""_ms( unsigned long long int msecs )
{
        return microseconds{ static_cast< uint32_t >( msecs * 1'000U ) };
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
        DISENGAGED,
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

}  // namespace servio
