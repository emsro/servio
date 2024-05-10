#include "fw/context.hpp"

#include "brd/brd.hpp"
#include "fw/util.hpp"

namespace servio::fw
{

context setup_context()
{
        cfg_context cfg_ctx = fw::load_cfg_context();

        core::drivers cdrv = brd::setup_core_drivers( cfg_ctx.map );
        if ( cdrv.any_uninitialized() )
                fw::stop_exec();

        context ctx{
            .cfg     = std::move( cfg_ctx ),
            .cdrv    = cdrv,
            .core    = { cdrv.clock->get_us(), *cdrv.vcc, *cdrv.temperature, *cdrv.clock },
            .scbs    = { *cdrv.motor, *cdrv.clock, ctx.core.ctl, ctx.core.met, ctx.core.conv },
            .cfg_dis = { ctx.cfg.map, ctx.core, *cdrv.motor },
        };
        ctx.scbs.set_callbacks( *cdrv.period, *cdrv.period_cb, *cdrv.position, *cdrv.current );
        ctx.cdrv.leds->update( ctx.core.ind.get_state() );

        ctx.cfg_dis.full_apply();

        if ( cdrv.start_cb( cdrv ) != em::SUCCESS )
                fw::stop_exec();

        ctx.core.ind.on_event( cdrv.clock->get_us(), mon::indication_event::INITIALIZED );

        return ctx;
}

}  // namespace servio::fw
