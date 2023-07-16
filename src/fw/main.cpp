#include "cfg/storage.hpp"
#include "fw/board.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/core.hpp"
#include "fw/dispatcher.hpp"
#include "fw/globals.hpp"
#include "fw/servio_pb.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/iterators/numeric.h>
#include <emlabcpp/match.h>
#include <emlabcpp/pid.h>
#include <string>

bool store_config(
    const em::view< const cfg::page* > pages,
    const cfg::payload&                pld,
    const cfg_map*                     cfg )
{
        std::optional< cfg::page > opt_page = cfg::find_next_page( pages );

        if ( !opt_page ) {
                return false;
        }

        const std::byte* start      = opt_page->begin();
        auto             start_addr = reinterpret_cast< uint32_t >( start );

        auto f = [&]( std::size_t offset, uint64_t val ) -> bool {
                HAL_FLASH_Unlock();
                const HAL_StatusTypeDef status =
                    HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, start_addr + offset, val );
                HAL_FLASH_Lock();
                return status == HAL_OK;
        };

        const bool succ = cfg::store( pld, cfg, f );

        return succ;
}

int main()
{

        brd::setup_board();

        cfg_map cfg = brd::get_config();

        em::view                   pages     = brd::get_persistent_pages();
        std::optional< cfg::page > last_page = cfg::find_latest_page( pages );
        // TODO: move the config logic into separate stuff so it can be used in tests
        cfg::payload last_cfg_payload{ .git_ver = "", .id = 0 };
        if ( last_page.has_value() ) {
                auto check_f = [&]( const cfg::payload& ) {
                        return true;
                };
                const bool cfg_loaded = cfg::load( *last_page, check_f, cfg );

                if ( !cfg_loaded ) {
                        fw::stop_exec();
                }
        }

        brd::core_drivers cdrv = brd::setup_core_drivers();

        if ( cdrv.any_uninitialized() ) {
                fw::stop_exec();
        }

        fw::core cor{ cdrv.clock->get_us(), *cdrv.vcc, *cdrv.temperature, *cdrv.clock };
        cdrv.leds->update( cor.ind.get_state() );

        const fw::standard_callbacks cbs(
            *cdrv.hbridge,
            *cdrv.clock,
            *cdrv.position,
            *cdrv.current,
            *cdrv.period_cb,
            cor.ctl,
            cor.met,
            cor.conv );

        fw::cfg_dispatcher cfg_dis{ cfg, cor };
        cfg_dis.full_apply();

        cdrv.start_cb( cdrv );

        cor.ind.on_event( cdrv.clock->get_us(), indication_event::INITIALIZED );

        std::byte input_buffer[HostToServioPacket_size];
        std::byte output_buffer[ServioToHostPacket_size];

        while ( true ) {
                cor.tick( *cdrv.leds, cdrv.clock->get_us() );

                auto write_config = [&]( const cfg_map* cfg ) -> bool {
                        const cfg::payload pld{
                            .git_ver = "",  // TODO: << fix this
                            .id      = last_cfg_payload.id + 1,
                        };
                        return store_config( pages, pld, cfg );
                };

                fw::dispatcher dis{
                    .hb         = *cdrv.hbridge,
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
                        cdrv.comms->send( odata );
                }
        }
}
