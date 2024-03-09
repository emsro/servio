#pragma once

#include "cfg/dispatcher.hpp"
#include "core/core.hpp"
#include "core/drivers.hpp"
#include "fw/cfg_context.hpp"
#include "fw/dispatcher.hpp"

namespace servio::fw
{

struct context
{
        fw::cfg_context          cfg;
        core::drivers            cdrv;
        core::core               core;
        core::standard_callbacks scbs;
        cfg::dispatcher          cfg_dis;

        void tick()
        {
                core.tick( *cdrv.leds, cdrv.clock->get_us() );
        }
};

context setup_context();

}  // namespace servio::fw
