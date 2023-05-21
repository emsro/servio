#include "cfg/storage.hpp"
#include "fw/board.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/core.hpp"
#include "fw/dispatcher.hpp"
#include "fw/globals.hpp"
#include "protocol.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/iterators/numeric.h>
#include <emlabcpp/match.h>
#include <emlabcpp/pid.h>
#include <string>

int main()
{
        brd::setup_board();

        cfg_map cfg = brd::get_config();

        em::view                   pages = brd::get_persistent_pages();
        std::optional< cfg::page > p     = cfg::find_latest_page( pages );
        if ( p.has_value() ) {
                auto check_f = [&]( const cfg::payload& ) {
                        return true;
                };
                bool cfg_loaded = cfg::load( p.value(), check_f, cfg );

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

        while ( true ) {
                fw::dispatcher dis{
                    *comms_ptr,
                    *hbridge_ptr,
                    *acquisition_ptr,
                    cor.ctl,
                    cfg_dis,
                    cor.conv,
                    clock_ptr->get_us() };
                em::match(
                    comms_ptr->get_message(),
                    []( std::monostate ) {},
                    [&]( const master_to_servo_variant& var ) {
                            em::match( var, [&]< typename T >( const T& item ) {
                                    dis.handle_message( item );
                            } );
                            cor.ind.on_event(
                                clock_ptr->get_us(), indication_event::INCOMING_MESSAGE );
                    },
                    [&]( const em::protocol::error_record& ) {
                            fw::stop_exec();
                    } );

                cor.tick( *leds_ptr, clock_ptr->get_us() );
        }
}
