#pragma once

#include "../cfg/dispatcher.hpp"
#include "../core/callbacks.hpp"
#include "../core/core.hpp"
#include "../core/drivers.hpp"

namespace servio::fw
{

struct context
{
        core::drivers            cdrv;
        core::core               core;
        core::standard_callbacks scbs;

        context( core::drivers cdrv )
          : cdrv( std::move( cdrv ) )
          , core( cdrv.clock->get_us(), *this->cdrv.vcc, *this->cdrv.temperature )
          , scbs(
                *this->cdrv.motor,
                *this->cdrv.clock,
                this->core.gov_,
                this->core.met,
                this->core.conv )
        {
        }

        void setup();

        void tick()
        {
                core.tick( *cdrv.leds, cdrv.clock->get_us() );
                cdrv.temperature->tick();
        }
};

context setup_context( core::drivers cdrv );

}  // namespace servio::fw
