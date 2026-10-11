#include "./context.hpp"

#include "../brd/brd.hpp"
#include "../status.hpp"
#include "./util.hpp"

namespace servio::fw
{

void context::setup()
{
        scbs.set_callbacks( *cdrv.period, *cdrv.period_cb, *cdrv.position, *cdrv.current );
        cdrv.leds->update( core.ind.get_state() );

        cdrv.cfg->handler.set_apply( em::member_function< &cfg::root_handler::apply >{ root_cfg } );
        for ( cfg::iface& c : core.govs.cfgs )
                c.apply_all();

        if ( cdrv.start_cb( cdrv ) != status::success )
                fw::stop_exec();

        core.ind.on_event( cdrv.clock->get_us(), mon::indication_event::INITIALIZED );
}

}  // namespace servio::fw
