#include "config/conversion.hpp"

#pragma once

class voltage_converter
{
public:
    voltage_converter( voltage_converter_config conf = { .scale = 1.f } )
      : conf_( conf )
    {
    }

    float convert( uint32_t val )
    {
        return static_cast< float >( val ) * conf_.scale;
    }

private:
    voltage_converter_config conf_;
};
