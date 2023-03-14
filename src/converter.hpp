
#include "conversion/current.hpp"
#include "conversion/position.hpp"
#include "conversion/temperature.hpp"
#include "conversion/voltage.hpp"

#pragma once

struct converter
{
        void set_position_cfg(
            uint16_t low_value,
            float    low_angle,
            uint16_t high_value,
            float    high_angle )
        {
                position = position_converter{ low_value, low_angle, high_value, high_angle };
        }
        void set_current_cfg( float scale, float offset )
        {
                current = current_converter{ scale, offset };
        }
        void set_temp_cfg( float scale, float offset )
        {
                temp = temperature_converter{ scale, offset };
        }
        void set_vcc_cfg( float scale )
        {
                vcc = voltage_converter{ scale };
        }

        position_converter    position;
        current_converter     current;
        temperature_converter temp;
        voltage_converter     vcc;
};
