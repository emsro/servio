#include <chrono>
#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <emlabcpp/quantity.h>
#include <numbers>

#pragma once

namespace em = emlabcpp;

namespace servio
{

template < typename T >
using opt = std::optional< T >;

using microseconds = std::chrono::duration< uint64_t, std::micro >;
using milliseconds = std::chrono::duration< uint64_t, std::milli >;
using seconds      = std::chrono::duration< uint64_t >;

constexpr microseconds operator""_s( unsigned long long int secs )
{
        return microseconds{ secs * 1'000'000U };
}

constexpr microseconds operator""_ms( unsigned long long int msecs )
{
        return microseconds{ msecs * 1'000U };
}

constexpr microseconds operator""_us( unsigned long long int usecs )
{
        return microseconds{ usecs };
}

constexpr std::byte operator""_b( unsigned long long int b )
{
        return std::byte{ static_cast< uint8_t >( b ) };
}

using sec_time = std::chrono::duration< float >;

template < typename T >
using limits = em::min_max< T >;

static constexpr float infty = std::numeric_limits< float >::infinity();
static constexpr float pi    = std::numbers::pi_v< float >;
static constexpr float pipi  = 2.F * std::numbers::pi_v< float >;

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
        uint8_t green;
};

struct pwr : em::quantity< pwr, float >
{
        using em::quantity< pwr, float >::quantity;
};

static constexpr pwr p_max = pwr{ 1.0 };
static constexpr pwr p_low = pwr{ -1.0 };

constexpr pwr operator""_pwr( unsigned long long int x )
{
        return pwr{ x };
}

constexpr pwr operator""_pwr( long double x )
{
        return pwr{ x };
}
}  // namespace servio
