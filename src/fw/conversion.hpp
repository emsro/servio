#include "converter.hpp"
#include "fw/drv/hbridge.hpp"

#pragma once

namespace fw
{

inline float current( const converter& conv, uint32_t raw_current, const drv::hbridge& hb )
{
        return conv.current.convert( raw_current ) * hb.get_direction();
}

inline float
current( const converter& conv, const drv::current_interface& curr_drv, const drv::hbridge& hb )
{
        return current( conv, curr_drv.get_current(), hb );
}

inline float position( const converter& conv, const drv::position_interface& pos_drv )
{
        return conv.position.convert( pos_drv.get_position() );
}

}  // namespace fw
