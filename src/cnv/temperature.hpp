#pragma once

namespace cnv
{

class temperature_converter
{
public:
        temperature_converter( float scale = 1.F, float offset = 0.F )
          : scale_( scale )
          , offset_( offset )
        {
        }

        float convert( uint32_t val ) const
        {
                return static_cast< float >( val ) * scale_ + offset_;
        }

private:
        float scale_;
        float offset_;
};

}  // namespace cnv
