#include "converter.hpp"
#include "fw/drv/acquisition.hpp"
#include "fw/drv/hbridge.hpp"

#pragma once

namespace fw
{

inline float current( const converter& conv, uint32_t raw_current, const drv::hbridge& hb )
{
        return conv.current.convert( raw_current ) * hb.get_direction();
}
inline float current( const converter& conv, const drv::acquisition& acqui, const drv::hbridge& hb )
{
        return current( conv, acqui.get_current(), hb );
}

inline float position( const converter& conv, const drv::acquisition& acqui )
{
        return conv.position.convert( acqui.get_position() );
}

}  // namespace fw
