
#include <cstdint>

#pragma once

namespace servio::cnv
{

struct linear_converter
{
        float convert( uint32_t input ) const
        {
                return static_cast< float >( input ) * scale + offset;
        }

        float convert( int32_t input ) const
        {
                return static_cast< float >( input ) * scale + offset;
        }

        float offset = 0.F;
        float scale  = 1.F;
};

}  // namespace servio::cnv
