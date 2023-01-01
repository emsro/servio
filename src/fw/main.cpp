#include "control.hpp"
#include "fw/board.hpp"
#include "fw/callbacks.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/dispatcher.hpp"
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
    fw::setup_board();

    fw::acquisition* acquisition_ptr = fw::setup_acquisition();
    fw::hbridge*     hbridge_ptr     = fw::setup_hbridge();
    fw::comms*       comms_ptr       = fw::setup_comms();
    fw::debug_comms* debug_comms_ptr = fw::setup_debug_comms();

    if ( acquisition_ptr == nullptr || hbridge_ptr == nullptr || comms_ptr == nullptr ||
         debug_comms_ptr == nullptr ) {
        fw::stop_exec();
    }

    hbridge_ptr->set_period_callback(
        fw::acquisition_period_callback{ *acquisition_ptr, *hbridge_ptr } );

    std::chrono::milliseconds now{ HAL_GetTick() };
    control                   ctl{ now };
    metrics                   met{ now, acquisition_ptr->get_position() };

    acquisition_ptr->set_current_callback( fw::current_callback{ *hbridge_ptr, ctl } );
    acquisition_ptr->set_position_callback( fw::position_callback{ ctl, met } );

    fw::cfg_dispatcher cfg_dis{ CONFIG, *acquisition_ptr, ctl };
    fw::apply_config( cfg_dis );

    acquisition_ptr->start();
    hbridge_ptr->start();
    comms_ptr->start_receiving();
    debug_comms_ptr->start_receiving();

    while ( true ) {
        now = std::chrono::milliseconds{ HAL_GetTick() };
        fw::dispatcher dis{ *comms_ptr, *acquisition_ptr, ctl, cfg_dis, now };
        em::match(
            comms_ptr->get_message(),
            []( std::monostate ) {},
            [&]( const master_to_servo_variant& var ) {
                em::match( var, [&]< typename T >( const T& item ) {
                    dis.handle_message( item );
                } );
            },
            [&]( const em::protocol::error_record& ) {
                fw::stop_exec();
            } );
    }
}
