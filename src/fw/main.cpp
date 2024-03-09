#include "brd.hpp"
#include "cfg/dispatcher.hpp"
#include "core/core.hpp"
#include "core/drivers.hpp"
#include "emlabcpp/result.h"
#include "fw/dispatcher.hpp"
#include "fw/servio_pb.hpp"
#include "fw/store_persistent_config.hpp"
#include "fw/util.hpp"
#include "load_persistent_config.hpp"

std::byte INPUT_BUFFER[HostToServioPacket_size];
std::byte OUTPUT_BUFFER[ServioToHostPacket_size];

int main()
{
        using namespace servio;
        using namespace servio::base::literals;

        if ( brd::setup_board() != em::SUCCESS )
                fw::stop_exec();

        cfg::map cfg = brd::get_default_config();

        em::view< const brd::page* > pages            = brd::get_persistent_pages();
        cfg::payload                 last_cfg_payload = fw::load_persistent_config( pages, cfg );

        core::drivers cdrv = brd::setup_core_drivers();
        if ( cdrv.any_uninitialized() )
                fw::stop_exec();

        core::core cor{ cdrv.clock->get_us(), *cdrv.vcc, *cdrv.temperature, *cdrv.clock };
        cdrv.leds->update( cor.ind.get_state() );

        core::standard_callbacks cbs( *cdrv.motor, *cdrv.clock, cor.ctl, cor.met, cor.conv );
        cbs.set_callbacks( *cdrv.period, *cdrv.period_cb, *cdrv.position, *cdrv.current );

        cfg::dispatcher cfg_dis{ cfg, cor };
        cfg_dis.full_apply();

        if ( cdrv.start_cb( cdrv ) != em::SUCCESS )
                fw::stop_exec();

        cor.ind.on_event( cdrv.clock->get_us(), mon::indication_event::INITIALIZED );

        while ( true ) {

                cor.tick( *cdrv.leds, cdrv.clock->get_us() );

                fw::persistent_config_writer cfg_writer{ last_cfg_payload, pages };

                fw::dispatcher dis{
                    .motor      = *cdrv.motor,
                    .pos_drv    = *cdrv.position,
                    .curr_drv   = *cdrv.current,
                    .vcc_drv    = *cdrv.vcc,
                    .temp_drv   = *cdrv.temperature,
                    .ctl        = cor.ctl,
                    .met        = cor.met,
                    .cfg_disp   = cfg_dis,
                    .cfg_writer = cfg_writer,
                    .conv       = cor.conv,
                    .now        = cdrv.clock->get_us() };

                auto [lsucc, ldata] = cdrv.comms->load_message( INPUT_BUFFER );

                if ( !lsucc )
                        fw::stop_exec();

                if ( ldata.empty() )
                        continue;

                cor.ind.on_event( cdrv.clock->get_us(), mon::indication_event::INCOMING_MESSAGE );

                auto [succ, odata] = fw::handle_message_packet( dis, ldata, OUTPUT_BUFFER );

                if ( succ == em::ERROR )
                        fw::stop_exec();
                if ( cdrv.comms->send( odata, 100_ms ) != em::SUCCESS )
                        fw::stop_exec();
        }
}
