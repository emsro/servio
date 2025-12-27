#pragma once

#include "../gov/governor_manager.hpp"
#include "../mon/indication.hpp"
#include "../mon/monitor.hpp"
#include "../mtr/metrics.hpp"

#include <emlabcpp/pmr/stack_resource.h>

namespace servio::core
{

struct core
{

        // XXX: could be utility?
        em::pmr::stack_resource< 1024 > gov_mem;

        gov::governor_manager gov_;
        cnv::converter        conv;
        mtr::metrics          met;
        mon::indication       ind;
        mon::monitor          mon;

        core( microseconds now, drv::vcc_iface const& vcc_drv, drv::temp_iface const& temp_drv )
          : gov_()
          , conv()
          , met( now, 0.F, 0.F, { 0.F, 2 * pi } )
          , ind()
          , mon( now, vcc_drv, temp_drv, ind, conv )
        {
        }

        void tick( drv::leds_iface& leds, microseconds now )
        {
                mon.tick( now, !!gov_.active() );
                ind.tick( now );
                leds.update( ind.get_state() );
        }
};

}  // namespace servio::core
