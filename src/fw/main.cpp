#include "cfg/storage.hpp"
#include "fw/board.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/core.hpp"
#include "fw/dispatcher.hpp"
#include "fw/globals.hpp"
#include "fw/servio_pb.hpp"
#include "load_persistent_config.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/defer.h>
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

        if ( HAL_FLASH_Unlock() != HAL_OK ) {
                fw::stop_exec();
        }
        const em::defer d = [] {
                if ( HAL_FLASH_Lock() != HAL_OK ) {
                        fw::stop_exec();
                }
        };

        const std::byte* start      = opt_page->begin();
        auto             start_addr = reinterpret_cast< uint32_t >( start );

        FLASH_EraseInitTypeDef erase_cfg{
            .TypeErase = FLASH_TYPEERASE_PAGES,
            .Banks     = FLASH_BANK_1,                                   // eeeeh, study this?
            .Page      = ( start_addr - FLASH_BASE ) / FLASH_PAGE_SIZE,  // this is hardcoded
                                                                         // too mcuh

            .NbPages = 1,
        };

        uint32_t                erase_err;
        const HAL_StatusTypeDef status = HAL_FLASHEx_Erase( &erase_cfg, &erase_err );
        if ( status != HAL_OK ) {
                fw::stop_exec();
        }
        if ( erase_err != 0xFFFFFFFF ) {
                fw::stop_exec();
        }

        auto f = [&]( std::size_t offset, uint64_t val ) -> bool {
                const HAL_StatusTypeDef status =
                    HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, start_addr + offset, val );
                if ( status != HAL_OK ) {
                        fw::stop_exec();
                }
                return status == HAL_OK;
        };

        const bool succ = cfg::store( pld, cfg, f );

        return succ;
}

int main()
{

        brd::setup_board();

        cfg_map cfg = brd::get_config();

        em::view pages = brd::get_persistent_pages();

        cfg::payload last_cfg_payload = load_persistent_config( pages, cfg );

        brd::core_drivers cdrv = brd::setup_core_drivers();

        if ( cdrv.any_uninitialized() ) {
                fw::stop_exec();
        }

        fw::core cor{ cdrv.clock->get_us(), *cdrv.vcc, *cdrv.temperature, *cdrv.clock };
        cdrv.leds->update( cor.ind.get_state() );

        const fw::standard_callbacks cbs(
            *cdrv.motor,
            *cdrv.period,
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
                        const bool succ = store_config( pages, pld, cfg );
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
                        cdrv.comms->send( odata );
                }
        }
}
