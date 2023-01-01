
#include <cstdint>

#pragma once

class current_converter
{
public:
    current_converter( float scale = 1.f, float offset = 0.f )
      : offset_( offset )
      , scale_( scale )
    {
    }

    float convert( float input )
    {
        return input * scale_ + offset_;
    }

private:
    float offset_;
    float scale_;
};
