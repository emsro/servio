#include "scaled.hpp"

#include <chrono>
#include <cstdint>

#pragma once

using protocol_power       = int16_t;
using protocol_current     = scaled< int32_t, 16 >;
using protocol_velocity    = scaled< int32_t, 16 >;
using protocol_position    = scaled< int32_t, 16 >;
using protocol_voltage     = scaled< int32_t, 16 >;
using protocol_temperature = scaled< int32_t, 16 >;

// time in milliseconds
using sec_time = std::chrono::duration< float >;

enum class control_mode
{
    POWER,
    CURRENT,
    VELOCITY,
    POSITION
};
