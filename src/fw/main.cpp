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
        cfg_map cfg = get_default_config();

        brd::setup_board();

        fw::leds*        leds_ptr        = fw::setup_leds_with_stop_callback();
        fw::acquisition* acquisition_ptr = brd::setup_acquisition();
        fw::hbridge*     hbridge_ptr     = brd::setup_hbridge();
        fw::comms*       comms_ptr       = brd::setup_comms();
        fw::debug_comms* debug_comms_ptr = brd::setup_debug_comms();

        if ( acquisition_ptr == nullptr || hbridge_ptr == nullptr || comms_ptr == nullptr ||
             debug_comms_ptr == nullptr || leds_ptr == nullptr ) {
                fw::stop_exec();
        }

        fw::core cor{ fw::ticks_ms(), *acquisition_ptr };
        leds_ptr->update( cor.ind.get_state() );

        fw::setup_standard_callbacks( *hbridge_ptr, *acquisition_ptr, cor.ctl, cor.met, cor.conv );

        fw::cfg_dispatcher cfg_dis{ cfg, cor };
        cfg_dis.full_apply();
        brd::apply_config( cfg_dis );

        fw::multistart( *leds_ptr, *acquisition_ptr, *hbridge_ptr, *comms_ptr, *debug_comms_ptr );

        cor.ind.on_event( fw::ticks_ms(), indication_event::INITIALIZED );

        fw::dispatcher dis{
            *comms_ptr,
            *hbridge_ptr,
            *acquisition_ptr,
            cor.ctl,
            cfg_dis,
            cor.conv,
            fw::ticks_ms() };

        while ( true ) {
                em::match(
                    comms_ptr->get_message(),
                    []( std::monostate ) {},
                    [&]( const master_to_servo_variant& var ) {
                            em::match( var, [&]< typename T >( const T& item ) {
                                    dis.handle_message( item );
                            } );
                            cor.ind.on_event( fw::ticks_ms(), indication_event::INCOMING_MESSAGE );
                    },
                    [&]( const em::protocol::error_record& ) {
                            fw::stop_exec();
                    } );

                cor.tick( *leds_ptr, fw::ticks_ms() );
        }
}
