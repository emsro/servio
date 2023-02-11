

#pragma once

class temperature_converter
{
public:
        temperature_converter( float scale = 1.f, float offset = 0.f )
          : scale_( scale )
          , offset_( offset )
        {
        }

        float convert( uint32_t val )
        {
                return static_cast< float >( val ) * scale_ + offset_;
        }

private:
        float scale_;
        float offset_;
};
