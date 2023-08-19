#include "brd.hpp"
#include "cfg_dispatcher.hpp"
#include "core.hpp"
#include "core_drivers.hpp"
#include "fw/board.hpp"
#include "fw/dispatcher.hpp"
#include "fw/servio_pb.hpp"
#include "fw/store_persistent_config.hpp"
#include "git.h"
#include "load_persistent_config.hpp"
#include "standard_callbacks.hpp"

int main()
{

        brd::setup_board();

        cfg_map cfg = brd::get_default_config();

        em::view pages = brd::get_persistent_pages();

        cfg::payload last_cfg_payload = load_persistent_config( pages, cfg );

        core_drivers cdrv = brd::setup_core_drivers();

        if ( cdrv.any_uninitialized() ) {
                fw::stop_exec();
        }

        core cor{ cdrv.clock->get_us(), *cdrv.vcc, *cdrv.temperature, *cdrv.clock };
        cdrv.leds->update( cor.ind.get_state() );

        standard_callbacks cbs( *cdrv.motor, *cdrv.clock, cor.ctl, cor.met, cor.conv );
        cbs.set_callbacks( *cdrv.period, *cdrv.period_cb, *cdrv.position, *cdrv.current );

        cfg_dispatcher cfg_dis{ cfg, cor };
        cfg_dis.full_apply();

        cdrv.start_cb( cdrv );

        cor.ind.on_event( cdrv.clock->get_us(), indication_event::INITIALIZED );

        std::byte input_buffer[HostToServioPacket_size];
        std::byte output_buffer[ServioToHostPacket_size];

        while ( true ) {

                cor.tick( *cdrv.leds, cdrv.clock->get_us() );

                auto write_config = [&]( const cfg_map* cfg ) -> bool {
                        const cfg::payload pld{
                            .git_ver  = git::Describe(),
                            .git_date = git::CommitDate(),
                            .id       = last_cfg_payload.id + 1,
                        };
                        std::optional< cfg::page > opt_page = cfg::find_next_page( pages );

                        if ( !opt_page ) {
                                return false;
                        }
                        const bool succ = fw::store_persistent_config( *opt_page, pld, cfg );
                        if ( succ ) {
                                last_cfg_payload = pld;
                        }
                        if ( cdrv.current->get_status() == status::DATA_ACQUISITION_ERROR ) {
                                cdrv.current->clear_status( status::DATA_ACQUISITION_ERROR );
                        }
                        return succ;
                };

                fw::dispatcher dis{
                    .motor      = *cdrv.motor,
                    .pos_drv    = *cdrv.position,
                    .curr_drv   = *cdrv.current,
                    .vcc_drv    = *cdrv.vcc,
                    .temp_drv   = *cdrv.temperature,
                    .ctl        = cor.ctl,
                    .met        = cor.met,
                    .cfg_disp   = cfg_dis,
                    .cfg_writer = write_config,
                    .conv       = cor.conv,
                    .now        = cdrv.clock->get_us() };

                auto [lsucc, ldata] = cdrv.comms->load_message( input_buffer );

                if ( !lsucc ) {
                        fw::stop_exec();
                }

                if ( ldata.empty() ) {
                        continue;
                }

                cor.ind.on_event( cdrv.clock->get_us(), indication_event::INCOMING_MESSAGE );

                auto [succ, odata] = fw::handle_message_packet( dis, ldata, output_buffer );

                if ( !succ ) {
                        // TODO: what now?
                } else {
                        // TODO: this should not be ignored
                        std::ignore = cdrv.comms->send( odata );
                }
        }
}
