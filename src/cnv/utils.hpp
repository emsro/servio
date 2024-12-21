#pragma once

#include "../drv/interfaces.hpp"
#include "./converter.hpp"

namespace servio::cnv
{
inline float
current( converter const& conv, uint32_t raw_current, drv::motor_info_iface const& motor )
{
        return conv.current.convert( raw_current ) * static_cast< float >( motor.get_direction() );
}

inline float current(
    converter const&             conv,
    drv::get_curr_iface const&   curr_drv,
    drv::motor_info_iface const& motor )
{
        return current( conv, curr_drv.get_current(), motor );
}

inline float position( converter const& conv, drv::get_pos_iface const& pos_drv )
{
        return conv.position.convert( pos_drv.get_position() );
}
}  // namespace servio::cnv
