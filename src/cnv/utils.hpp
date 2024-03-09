#pragma once

#include "cnv/converter.hpp"
#include "drv/interfaces.hpp"

namespace servio::cnv
{
inline float
current( const converter& conv, uint32_t raw_current, const drv::pwm_motor_interface& motor )
{
        return conv.current.convert( raw_current ) * static_cast< float >( motor.get_direction() );
}

inline float current(
    const converter&                conv,
    const drv::current_interface&   curr_drv,
    const drv::pwm_motor_interface& motor )
{
        return current( conv, curr_drv.get_current(), motor );
}

inline float position( const converter& conv, const drv::position_interface& pos_drv )
{
        return conv.position.convert( pos_drv.get_position() );
}
}  // namespace servio::cnv
