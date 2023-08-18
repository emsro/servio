#pragma once

#include "converter.hpp"
#include "drv_interfaces.hpp"

namespace cnv
{
inline float
current( const converter& conv, uint32_t raw_current, const pwm_motor_interface& motor )
{
        return conv.current.convert( raw_current ) * static_cast< float >( motor.get_direction() );
}

inline float current(
    const converter&           conv,
    const current_interface&   curr_drv,
    const pwm_motor_interface& motor )
{
        return current( conv, curr_drv.get_current(), motor );
}

inline float position( const converter& conv, const position_interface& pos_drv )
{
        return conv.position.convert( pos_drv.get_position() );
}
}  // namespace cnv
