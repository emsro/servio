#include "cnv/current.hpp"
#include "cnv/position.hpp"
#include "cnv/temperature.hpp"
#include "cnv/voltage.hpp"

#pragma once

namespace servio
{

struct converter
{
        void set_position_cfg(
            uint32_t low_value,
            float    low_angle,
            uint32_t high_value,
            float    high_angle )
        {
                position = cnv::position_converter{ low_value, low_angle, high_value, high_angle };
        }
        void set_current_cfg( float scale, float offset )
        {
                current = cnv::current_converter{ scale, offset };
        }
        void set_temp_cfg( float scale, float offset )
        {
                temp = cnv::temperature_converter{ scale, offset };
        }
        void set_vcc_cfg( float scale )
        {
                vcc = cnv::voltage_converter{ scale };
        }

        cnv::position_converter    position;
        cnv::current_converter     current;
        cnv::temperature_converter temp;
        cnv::voltage_converter     vcc;
};

}  // namespace servio
