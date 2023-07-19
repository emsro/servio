#include <cstdint>

#pragma once

namespace cnv
{

class current_converter
{
public:
        current_converter( float scale = 1.f, float offset = 0.f )
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

}  // namespace cnv
