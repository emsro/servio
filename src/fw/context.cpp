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

        cfg::root_handler rh{
            .m     = cdrv.cfg->map,
            .conv  = core.conv,
            .met   = core.met,
            .mon   = core.mon,
            .motor = *cdrv.motor,
            .pos   = *cdrv.position };

        rh.full_apply();

        if ( cdrv.start_cb( cdrv ) != SUCCESS )
                fw::stop_exec();

        core.ind.on_event( cdrv.clock->get_us(), mon::indication_event::INITIALIZED );

        gov::create_governors( core.gov_, core.gov_mem );
}

}  // namespace servio::fw
