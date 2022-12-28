#include "scaled.hpp"

#include <emlabcpp/algorithm.h>
#include <chrono>
#include <cstdint>

#pragma once

namespace em = emlabcpp;

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
