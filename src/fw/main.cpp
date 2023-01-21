#include "control.hpp"
#include "fw/board.hpp"
#include "fw/callbacks.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/dispatcher.hpp"
#include "fw/globals.hpp"
#include "fw/monitor.hpp"
#include "indication.hpp"
#include "metrics.hpp"
#include "protocol.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/iterators/numeric.h>
#include <emlabcpp/match.h>
#include <emlabcpp/pid.h>
#include <string>

cfg_map CONFIG;

int main()
{
    brd::setup_board();

    fw::leds* leds_ptr = brd::setup_leds();
    if ( leds_ptr != nullptr ) {
        fw::STOP_CALLBACK = [leds_ptr] {
            leds_ptr->force_red_led();
        };
    }
    fw::acquisition* acquisition_ptr = brd::setup_acquisition();
    fw::hbridge*     hbridge_ptr     = brd::setup_hbridge();
    fw::comms*       comms_ptr       = brd::setup_comms();
    fw::debug_comms* debug_comms_ptr = brd::setup_debug_comms();

    if ( acquisition_ptr == nullptr || hbridge_ptr == nullptr || comms_ptr == nullptr ||
         debug_comms_ptr == nullptr || leds_ptr == nullptr ) {
        fw::stop_exec();
    }

    control     ctl{ fw::ticks_ms() };
    metrics     met{ fw::ticks_ms(), acquisition_ptr->get_position() };
    indication  indi{ fw::ticks_ms() };
    fw::monitor mon{ fw::ticks_ms(), ctl, *acquisition_ptr, indi };

    indi.tick( fw::ticks_ms() );
    leds_ptr->update( indi.get_state() );

    hbridge_ptr->set_period_callback(
        fw::acquisition_period_callback{ *acquisition_ptr, *hbridge_ptr } );
    acquisition_ptr->set_current_callback( fw::current_callback{ *hbridge_ptr, ctl } );
    acquisition_ptr->set_position_callback( fw::position_callback{ ctl, met } );

    fw::cfg_dispatcher cfg_dis{ CONFIG, *acquisition_ptr, ctl, mon };
    brd::apply_config( cfg_dis );

    acquisition_ptr->start();
    hbridge_ptr->start();
    comms_ptr->start_receiving();
    debug_comms_ptr->start_receiving();

    indi.on_event( fw::ticks_ms(), indication_event::INITIALIZED );

    while ( true ) {
        fw::dispatcher dis{ *comms_ptr, *acquisition_ptr, ctl, cfg_dis, fw::ticks_ms() };
        em::match(
            comms_ptr->get_message(),
            []( std::monostate ) {},
            [&]( const master_to_servo_variant& var ) {
                em::match( var, [&]< typename T >( const T& item ) {
                    dis.handle_message( item );
                } );
                indi.on_event( fw::ticks_ms(), indication_event::INCOMING_MESSAGE );
            },
            [&]( const em::protocol::error_record& ) {
                fw::stop_exec();
            } );

        mon.tick( fw::ticks_ms() );
        indi.tick( fw::ticks_ms() );
        leds_ptr->update( indi.get_state() );
    }
}
