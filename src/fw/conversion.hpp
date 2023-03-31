
#include "converter.hpp"
#include "fw/drivers/acquisition.hpp"
#include "fw/drivers/hbridge.hpp"

#pragma once

namespace fw
{

inline float current( const converter& conv, uint32_t raw_current, const hbridge& hb )
{
        return conv.current.convert( raw_current ) * hb.get_direction();
}
inline float current( const converter& conv, const acquisition& acqui, const hbridge& hb )
{
        return current( conv, acqui.get_current(), hb );
}

inline float position( const converter& conv, const acquisition& acqui )
{
        return conv.position.convert( acqui.get_position() );
}

}  // namespace fw
