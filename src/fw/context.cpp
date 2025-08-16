#include "./context.hpp"

#include "../brd/brd.hpp"
#include "../status.hpp"
#include "./util.hpp"

namespace servio::fw
{

context setup_context()
{

        core::drivers cdrv = brd::setup_core_drivers();
        if ( cdrv.any_uninitialized() )
                fw::stop_exec();

        context ctx{
            .cdrv = cdrv,
            .core = { cdrv.clock->get_us(), *cdrv.vcc, *cdrv.temperature },
            .scbs = { *cdrv.motor, *cdrv.clock, ctx.core.gov_, ctx.core.met, ctx.core.conv },
        };
        ctx.scbs.set_callbacks( *cdrv.period, *cdrv.period_cb, *cdrv.position, *cdrv.current );
        ctx.cdrv.leds->update( ctx.core.ind.get_state() );

        cfg::dispatcher cfg_dis{
            cdrv.cfg->map, ctx.core.conv, ctx.core.met, ctx.core.mon, *cdrv.motor, *cdrv.position };
        cfg_dis.full_apply();

        if ( cdrv.start_cb( cdrv ) != SUCCESS )
                fw::stop_exec();

        ctx.core.ind.on_event( cdrv.clock->get_us(), mon::indication_event::INITIALIZED );

        return ctx;
}

}  // namespace servio::fw
