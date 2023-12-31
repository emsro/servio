#pragma once

#include "base/drv_interfaces.hpp"
#include "cnv/converter.hpp"

namespace servio::cnv
{
inline float
current( const converter& conv, uint32_t raw_current, const base::pwm_motor_interface& motor )
{
        return conv.current.convert( raw_current ) * static_cast< float >( motor.get_direction() );
}

inline float current(
    const converter&                 conv,
    const base::current_interface&   curr_drv,
    const base::pwm_motor_interface& motor )
{
        return current( conv, curr_drv.get_current(), motor );
}

inline float position( const converter& conv, const base::position_interface& pos_drv )
{
        return conv.position.convert( pos_drv.get_position() );
}
}  // namespace servio::cnv
