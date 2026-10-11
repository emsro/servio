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
        cfg::root_handler        root_cfg;

        context( core::drivers cdrv, gov::governors_ctx& govs )
          : cdrv( std::move( cdrv ) )
          , core( govs, this->cdrv.clock->get_us(), *this->cdrv.vcc, *this->cdrv.temperature )
          , scbs(
                *this->cdrv.motor,
                *this->cdrv.clock,
                this->core.govs.manager,
                this->core.met,
                this->core.conv )
          , root_cfg{
                .m     = this->cdrv.cfg->map,
                .conv  = this->core.conv,
                .met   = this->core.met,
                .mon   = this->core.mon,
                .motor = *this->cdrv.motor,
                .pos   = *this->cdrv.position }
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
