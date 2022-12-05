
#include "config/conversion.hpp"

#pragma once

class temperature_converter
{
public:
    temperature_converter( temperature_converter_config conf = { .scale = 1.f, .offset = 0.f } )
      : conf_( conf )
    {
    }

    float convert( uint32_t val )
    {
        return static_cast< float >( val ) * conf_.scale + conf_.offset;
    }

private:
    temperature_converter_config conf_;
};
