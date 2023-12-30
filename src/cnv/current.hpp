#include <cstdint>

#pragma once

namespace servio::cnv
{

class current_converter
{
public:
        current_converter( float scale = 1.F, float offset = 0.F )
          : offset_( offset )
          , scale_( scale )
        {
        }

        float convert( uint32_t input ) const
        {
                return static_cast< float >( input ) * scale_ + offset_;
        }

private:
        float offset_;
        float scale_;
};

}  // namespace servio::cnv
