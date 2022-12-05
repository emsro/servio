#include "control.hpp"
#include "fw/board.hpp"
#include "fw/callbacks.hpp"
#include "fw/dispatcher.hpp"
#include "metrics.hpp"
#include "protocol.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/iterators/numeric.h>
#include <emlabcpp/match.h>
#include <emlabcpp/pid.h>
#include <string>

int main()
{
    fw::setup_board();

    config cfg;
    fw::setup_config( cfg );

    fw::acquisition* acquisition_ptr = fw::setup_acquisition( cfg );
    fw::hbridge*     hbridge_ptr     = fw::setup_hbridge( cfg );
    fw::comms*       comms_ptr       = fw::setup_comms();
    fw::debug_comms* debug_comms_ptr = fw::setup_debug_comms();

    if ( acquisition_ptr == nullptr || hbridge_ptr == nullptr || comms_ptr == nullptr ||
         debug_comms_ptr == nullptr ) {
        fw::stop_exec();
    }

    hbridge_ptr->set_period_callback(
        fw::acquisition_period_callback{ *acquisition_ptr, *hbridge_ptr } );

    acquisition_ptr->start();
    hbridge_ptr->start();

    const master_to_servo_message* msg_ptr;

    auto comms_cb = [&]( const master_to_servo_message& msg ) {
        msg_ptr = &msg;
    };
    comms_ptr->set_msg_callback( comms_cb );
    debug_comms_ptr->start_receiving();

    std::chrono::milliseconds now{ HAL_GetTick() };
    control                   ctl{ now };
    metrics                   met{ now, acquisition_ptr->get_position() };

    acquisition_ptr->set_current_callback( fw::current_callback{ *hbridge_ptr, ctl } );
    acquisition_ptr->set_position_callback( fw::position_callback{ ctl, met } );

    while ( true ) {
        while ( msg_ptr == nullptr ) {
            now = std::chrono::milliseconds{ HAL_GetTick() };
        }
        master_to_servo_message msg = *msg_ptr;
        msg_ptr                     = nullptr;

        master_to_servo_deserialize( msg ).match(
            [&]( master_to_servo_variant var ) {
                fw::dispatcher dis{ *comms_ptr, *acquisition_ptr, ctl, now };
                em::match( var, [&]< typename T >( T item ) {
                    dis.handle_message( item );
                } );
            },
            [&]( em::protocol::error_record e ) {
                std::ignore = e;
                fw::stop_exec();
            } );
    }
}
