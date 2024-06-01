#include "brd/brd.hpp"
#include "fw/context.hpp"
#include "fw/servio_pb.hpp"
#include "fw/store_persistent_config.hpp"

std::byte INPUT_BUFFER[HostToServioPacket_size + 32];
std::byte OUTPUT_BUFFER[ServioToHostPacket_size + 32];

int main()
{
        using namespace servio;

        if ( brd::setup_board() != em::SUCCESS )
                fw::stop_exec();

        fw::context ctx = fw::setup_context();
        while ( true ) {
                ctx.tick();

                fw::persistent_config_writer cfg_writer{ ctx.cdrv.cfg->payload, *ctx.cdrv.storage };

                fw::dispatcher dis{
                    .motor      = *ctx.cdrv.motor,
                    .pos_drv    = *ctx.cdrv.position,
                    .curr_drv   = *ctx.cdrv.current,
                    .vcc_drv    = *ctx.cdrv.vcc,
                    .temp_drv   = *ctx.cdrv.temperature,
                    .ctl        = ctx.core.ctl,
                    .met        = ctx.core.met,
                    .cfg_disp   = ctx.cfg_dis,
                    .cfg_writer = cfg_writer,
                    .conv       = ctx.core.conv,
                    .now        = ctx.cdrv.clock->get_us() };

                auto [lsucc, ldata] = ctx.cdrv.comms->recv( INPUT_BUFFER );

                if ( !lsucc )
                        fw::stop_exec();

                if ( ldata.empty() )
                        continue;

                ctx.core.ind.on_event(
                    ctx.cdrv.clock->get_us(), mon::indication_event::INCOMING_MESSAGE );

                auto [succ, odata] = fw::handle_message_packet( dis, ldata, OUTPUT_BUFFER );

                if ( succ == em::ERROR )
                        fw::stop_exec();
                if ( send( *ctx.cdrv.comms, 100_ms, odata ) != em::SUCCESS )
                        fw::stop_exec();
        }
}
