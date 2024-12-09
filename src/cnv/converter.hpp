#include "./linear_converter.hpp"

#pragma once

namespace servio::cnv
{

struct converter
{
        void set_position_cfg(
            uint32_t low_value,
            float    low_angle,
            uint32_t high_value,
            float    high_angle )
        {
                int const   val_diff   = static_cast< int >( high_value - low_value );
                float const angle_diff = high_angle - low_angle;
                position.scale         = angle_diff / static_cast< float >( val_diff );
                position.offset = low_angle - static_cast< float >( low_value ) * position.scale;
        }

        void set_current_cfg( float scale, float offset )
        {
                current.offset = offset;
                current.scale  = scale;
        }

        void set_temp_cfg( float scale, float offset )
        {
                temp.offset = offset;
                temp.scale  = scale;
        }

        void set_vcc_cfg( float scale )
        {
                vcc.scale = scale;
        }

        linear_converter position;
        linear_converter current;
        linear_converter temp;
        linear_converter vcc;
};

}  // namespace servio::cnv
