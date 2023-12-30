#include <cstdint>

#pragma once

namespace servio::cnv
{

class voltage_converter
{
public:
        voltage_converter( float scale = 1.F )
          : scale_( scale )
        {
        }

        float convert( uint32_t val ) const
        {
                return static_cast< float >( val ) * scale_;
        }

private:
        float scale_;
};

}  // namespace servio::cnv
