
#include "ftest/rewind.hpp"

namespace servio::ftest
{

void rewind(
    core::core&          cor,
    drv::clk_iface&      iclk,
    drv::pos_iface&      ipos,
    base::microseconds   timeout,
    em::min_max< float > area,
    float                current )
{
        base::microseconds end = iclk.get_us() + timeout;

        while ( iclk.get_us() < end ) {
                float pos = cor.conv.position.convert( ipos.get_position() );
                if ( em::contains( area, pos ) )
                        break;
                float dir = area.max() < pos ? -1 : 1;
                cor.ctl.switch_to_current_control( iclk.get_us(), dir * current );
        }
        cor.ctl.switch_to_current_control( iclk.get_us(), 0.f );
}

}  // namespace servio::ftest
