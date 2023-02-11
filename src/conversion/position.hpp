
#include <cstdint>

#pragma once

class position_converter
{
public:
        position_converter(
            uint16_t lower_value  = 0,
            float    lower_angle  = 0.f,
            uint16_t higher_value = 4096,
            float    higher_angle = 3.141592f )
        {
                int   val_diff   = higher_value - lower_value;
                float angle_diff = higher_angle - lower_angle;
                scale_           = angle_diff / static_cast< float >( val_diff );
                offset_          = lower_angle - lower_value * scale_;
        }

        float convert( uint32_t val )
        {
                return static_cast< float >( val ) * scale_ + offset_;
        }

private:
        float scale_;
        float offset_;
};
