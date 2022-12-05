#include <cstdint>

#pragma once

struct voltage_converter_config
{
    float scale;
};

struct temperature_converter_config
{
    float scale;
    float offset;
};

struct position_converter_config
{
    struct setpoint
    {
        uint16_t value;
        float    angle;
    };

    setpoint lower_setpoint;
    setpoint higher_setpoint;
};

struct current_converter_config
{
    float offset;
    float scale;
};
