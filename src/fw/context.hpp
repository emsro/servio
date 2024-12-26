#pragma once

#include "../cfg/dispatcher.hpp"
#include "../core/core.hpp"
#include "../core/drivers.hpp"

namespace servio::fw
{

struct context
{
        core::drivers            cdrv;
        core::core               core;
        core::standard_callbacks scbs;

        void tick()
        {
                core.tick( *cdrv.leds, cdrv.clock->get_us() );
                cdrv.temperature->tick();
        }
};

context setup_context();

}  // namespace servio::fw
