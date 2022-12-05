#include "config/conversion.hpp"

#pragma once

// TODO: the config should not leak implementation details
struct config
{
    bool                         reversed;
    position_converter_config    position_conv;
    current_converter_config     current_conv;
    temperature_converter_config temp_conv;
    voltage_converter_config     volt_conv;
};
