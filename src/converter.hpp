
#include "conversion/current.hpp"
#include "conversion/position.hpp"
#include "conversion/temperature.hpp"
#include "conversion/voltage.hpp"

#pragma once

class converter
{
public:
        void set_position_cfg(
            uint16_t low_value,
            float    low_angle,
            uint16_t high_value,
            float    high_angle )
        {
                pos_conver_ = position_converter{ low_value, low_angle, high_value, high_angle };
        }
        float convert_position( uint32_t val ) const
        {
                return pos_conver_.convert( val );
        }

        void set_current_cfg( float scale, float offset )
        {
                curr_conver_ = current_converter{ scale, offset };
        }
        float convert_current( uint32_t val, int8_t direction ) const
        {
                return curr_conver_.convert( static_cast< float >( val ) ) * direction;
        }

        void set_temp_cfg( float scale, float offset )
        {
                temp_conver_ = temperature_converter{ scale, offset };
        }
        float convert_temp( uint32_t val ) const
        {
                return temp_conver_.convert( val );
        }
        void set_vcc_cfg( float scale )
        {
                volt_conver_ = voltage_converter{ scale };
        }
        float convert_vcc( uint32_t val ) const
        {
                return volt_conver_.convert( val );
        }

private:
        position_converter    pos_conver_;
        current_converter     curr_conver_;
        temperature_converter temp_conver_;
        voltage_converter     volt_conver_;
};
