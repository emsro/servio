#include "config/conversion.hpp"

#include <cstdint>

#pragma once

class current_converter
{
public:
    current_converter( current_converter_config conf = { .offset = 0.f, .scale = 1.f } )
      : config_( conf )
    {
    }

    float convert( float input )
    {
        return input * config_.scale + config_.offset;
    }

private:
    current_converter_config config_;
};
