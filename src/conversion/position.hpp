#include "config/conversion.hpp"

#include <cstdint>

#pragma once

class position_converter
{
public:
    position_converter(
        position_converter_config config = {
            .lower_setpoint  = { .value = 0, .angle = 0.f },
            .higher_setpoint = { .value = 4096, .angle = 3.141592f } } )
    {
        int   val_diff   = config.higher_setpoint.value - config.lower_setpoint.value;
        float angle_diff = config.higher_setpoint.angle - config.lower_setpoint.angle;
        scale_           = angle_diff / static_cast< float >( val_diff );
        offset_          = config.lower_setpoint.angle - config.lower_setpoint.value * scale_;
    }

    float convert( uint32_t val )
    {
        return static_cast< float >( val ) * scale_ + offset_;
    }

private:
    float scale_;
    float offset_;
};
