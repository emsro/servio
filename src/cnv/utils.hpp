#pragma once

#include "cnv/converter.hpp"
#include "drv/interfaces.hpp"

namespace servio::cnv
{
inline float
current( const converter& conv, uint32_t raw_current, const drv::pwm_motor_iface& motor )
{
        return conv.current.convert( raw_current ) * static_cast< float >( motor.get_direction() );
}

inline float
current( const converter& conv, const drv::curr_iface& curr_drv, const drv::pwm_motor_iface& motor )
{
        return current( conv, curr_drv.get_current(), motor );
}

inline float position( const converter& conv, const drv::pos_iface& pos_drv )
{
        return conv.position.convert( pos_drv.get_position() );
}
}  // namespace servio::cnv
