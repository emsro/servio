#include "../brd/brd.hpp"
#include "../core/dispatcher.hpp"
#include "./context.hpp"
#include "./util.hpp"

std::byte INPUT_BUFFER[128];
std::byte OUTPUT_BUFFER[128];

int main()
{
        using namespace servio;

        if ( brd::setup_board() != em::SUCCESS )
                fw::stop_exec();

        fw::context ctx = fw::setup_context();
        while ( true ) {
                ctx.tick();

                core::dispatcher dis{
                    .motor    = *ctx.cdrv.motor,
                    .pos_drv  = *ctx.cdrv.position,
                    .curr_drv = *ctx.cdrv.current,
                    .vcc_drv  = *ctx.cdrv.vcc,
                    .temp_drv = *ctx.cdrv.temperature,
                    .ctl      = ctx.core.ctl,
                    .met      = ctx.core.met,
                    .cfg_disp = ctx.cfg_dis,
                    .cfg_pl   = ctx.cdrv.cfg->payload,
                    .stor_drv = *ctx.cdrv.storage,
                    .conv     = ctx.core.conv,
                    .now      = ctx.cdrv.clock->get_us() };

                auto [lsucc, ldata] = ctx.cdrv.comms->recv( INPUT_BUFFER );

                if ( !lsucc )
                        fw::stop_exec();

                if ( ldata.empty() )
                        continue;

                ctx.core.ind.on_event(
                    ctx.cdrv.clock->get_us(), mon::indication_event::INCOMING_MESSAGE );

                // XXX: packet handling
                auto [succ, odata] = core::handle_message( dis, ldata, OUTPUT_BUFFER );

                if ( succ == em::ERROR )
                        fw::stop_exec();
                if ( send( *ctx.cdrv.comms, 100_ms, odata ) != em::SUCCESS )
                        fw::stop_exec();
        }
}
