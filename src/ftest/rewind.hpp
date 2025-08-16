#pragma once

#include "../core/core.hpp"
#include "../core/drivers.hpp"
#include "../fw/util.hpp"
#include "../gov/current/current.hpp"
#include "../gov/power/power.hpp"

namespace servio::ftest
{

namespace em = emlabcpp;

void rewind(
    core::core&          cor,
    drv::clk_iface&      iclk,
    drv::pos_iface&      ipos,
    microseconds         timeout,
    em::min_max< float > area,
    float                current,
    auto                 cb )
{
        microseconds end = iclk.get_us() + timeout;

        cor.gov_.activate( "current", {} );
        auto* p = dynamic_cast< gov::curr::_current_gov* >( cor.gov_.active_governor() );
        if ( !p )
                fw::stop_exec();

        while ( iclk.get_us() < end ) {
                float pos = cor.conv.position.convert( ipos.get_position() );
                if ( em::contains( area, pos ) )
                        break;
                float dir = area.max() < pos ? -1 : 1;

                p->set_goal_current( dir * current );
                cb();
        }
        p->set_goal_current( 0 );
        end = iclk.get_us() + 150_ms;
        while ( iclk.get_us() < end )
                cb();
        if ( cor.gov_.deactivate() != SUCCESS )
                fw::stop_exec();
}

}  // namespace servio::ftest
