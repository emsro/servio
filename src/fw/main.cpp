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

bool store_config( const cfg::page& page, const cfg::payload& pld, const cfg_map* cfg )
{
        const std::byte* start      = page.begin();
        uint32_t         start_addr = reinterpret_cast< uint32_t >( start );

        auto f = [&]( std::size_t offset, uint64_t val ) -> bool {
                HAL_FLASH_Unlock();
                HAL_StatusTypeDef status =
                    HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, start_addr + offset, val );
                HAL_FLASH_Lock();
                return status == HAL_OK;
        };

        bool succ = cfg::store( pld, cfg, f );

        return succ;
}

int main()
{
        brd::setup_board();

        cfg_map cfg = brd::get_config();

        em::view                   pages     = brd::get_persistent_pages();
        std::optional< cfg::page > last_page = cfg::find_latest_page( pages );
        cfg::payload               last_cfg_payload{ .git_ver = "", .id = 0 };
        if ( last_page.has_value() ) {
                auto check_f = [&]( const cfg::payload& ) {
                        return true;
                };
                bool cfg_loaded = cfg::load( *last_page, check_f, cfg );

                if ( !cfg_loaded ) {
                        fw::stop_exec();
                }
        }

        fw::clock*       clock_ptr       = brd::setup_clock();
        fw::leds*        leds_ptr        = fw::setup_leds_with_stop_callback();
        fw::acquisition* acquisition_ptr = brd::setup_acquisition();
        fw::hbridge*     hbridge_ptr     = brd::setup_hbridge();
        fw::comms*       comms_ptr       = brd::setup_comms();
        fw::debug_comms* debug_comms_ptr = brd::setup_debug_comms();

        if ( clock_ptr == nullptr || acquisition_ptr == nullptr || hbridge_ptr == nullptr ||
             comms_ptr == nullptr || debug_comms_ptr == nullptr || leds_ptr == nullptr ) {
                fw::stop_exec();
        }

        fw::core cor{ clock_ptr->get_us(), *acquisition_ptr, *clock_ptr };
        leds_ptr->update( cor.ind.get_state() );

        fw::standard_callbacks cbs(
            *hbridge_ptr, *acquisition_ptr, *clock_ptr, cor.ctl, cor.met, cor.conv );

        fw::cfg_dispatcher cfg_dis{ cfg, cor };
        cfg_dis.full_apply();

        fw::multistart( *leds_ptr, *acquisition_ptr, *hbridge_ptr, *comms_ptr, *debug_comms_ptr );

        cor.ind.on_event( clock_ptr->get_us(), indication_event::INITIALIZED );

        std::byte imsg[HostToServio_size];

        while ( true ) {
                cor.tick( *leds_ptr, clock_ptr->get_us() );

                std::optional< cfg::page > opt_page = cfg::find_next_page( pages );

                if ( !opt_page ) {
                        // TODO: well this is aggresive /o\...
                        fw::stop_exec();
                }

                auto write_config = [&]( const cfg_map* cfg ) -> bool {
                        cfg::payload pld{
                            .git_ver = "",  // TODO: << fix this
                            .id      = last_cfg_payload.id + 1,
                        };
                        return store_config( *opt_page, pld, cfg );
                };

                fw::dispatcher dis{
                    .hb         = *hbridge_ptr,
                    .acquis     = *acquisition_ptr,
                    .ctl        = cor.ctl,
                    .cfg_disp   = cfg_dis,
                    .cfg_writer = write_config,
                    .conv       = cor.conv,
                    .now        = clock_ptr->get_us() };

                auto [lsucc, ldata] = comms_ptr->load_message( imsg );

                if ( !lsucc ) {
                        fw::stop_exec();
                }

                if ( ldata.empty() ) {
                        continue;
                }

                cor.ind.on_event( clock_ptr->get_us(), indication_event::INCOMING_MESSAGE );

                HostToServio msg  = {};
                bool         succ = fw::decode( ldata, msg );

                ServioToHost reply = {};
                if ( !succ ) {
                        reply = fw::error_msg( "invalid message" );
                } else {
                        reply = handle_message( dis, msg );
                }

                std::byte buffer[ServioToHost_size];
                auto [esucc, data] = fw::encode( buffer, reply );
                if ( !esucc ) {
                        // TODO: well this is aggresive
                        fw::stop_exec();
                }

                comms_ptr->send( data );
        }
}
