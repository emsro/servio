#include <cstdint>

#pragma once

namespace servio::cnv
{

class position_converter
{
public:
        position_converter(
            uint32_t lower_value  = 0,
            float    lower_angle  = 0.F,
            uint32_t higher_value = 4096,
            float    higher_angle = 3.141592F )
        {
                const int   val_diff   = static_cast< int >( higher_value - lower_value );
                const float angle_diff = higher_angle - lower_angle;
                scale_                 = angle_diff / static_cast< float >( val_diff );
                offset_                = lower_angle - static_cast< float >( lower_value ) * scale_;
        }

        float convert( uint32_t val ) const
        {
                return static_cast< float >( val ) * scale_ + offset_;
        }

private:
        float scale_;
        float offset_;
};

}  // namespace servio::cnv
