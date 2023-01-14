#include "scaled.hpp"

#include <chrono>
#include <cstdint>
#include <emlabcpp/algorithm.h>

#pragma once

namespace em = emlabcpp;

using namespace std::chrono_literals;

using protocol_power       = int16_t;
using protocol_current     = scaled< int32_t, 16 >;
using protocol_velocity    = scaled< int32_t, 16 >;
using protocol_position    = scaled< int32_t, 16 >;
using protocol_voltage     = scaled< int32_t, 16 >;
using protocol_temperature = scaled< int32_t, 16 >;

// time in milliseconds
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

