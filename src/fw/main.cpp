#include "fw/board.hpp"
#include "fw/callbacks.hpp"
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

consteval cfg_map get_default_config()
{
    return cfg_map{
        cfg_reg< REVERSED, bool >{ false },
        cfg_reg< POSITION_CONV_LOWER_SETPOINT_VALUE, uint16_t >{ 0 },
        cfg_reg< POSITION_CONV_LOWER_SETPOINT_ANGLE, float >{ 0.f },
        cfg_reg< POSITION_CONV_HIGHER_SETPOINT_VALUE, uint16_t >{ 0 },
        cfg_reg< POSITION_CONV_HIGHER_SETPOINT_ANGLE, float >{ 0.f },
        cfg_reg< CURRENT_CONV_SCALE, float >{ 0.f },
        cfg_reg< CURRENT_CONV_OFFSET, float >{ 0.f },
        cfg_reg< TEMP_CONV_SCALE, float >{ 0.f },
        cfg_reg< TEMP_CONV_OFFSET, float >{ 0.f },
        cfg_reg< VOLTAGE_CONV_SCALE, float >{ 0.f },
        cfg_reg< CONTROL_CURRENT_LOOP_P, float >{ 0.f },
        cfg_reg< CONTROL_CURRENT_LOOP_I, float >{ 0.f },
        cfg_reg< CONTROL_CURRENT_LOOP_D, float >{ 0.f },
        cfg_reg< CONTROL_CURRENT_LIM_MIN, float >{ 0.f },
        cfg_reg< CONTROL_CURRENT_LIM_MAX, float >{ 0.f },
        cfg_reg< CONTROL_VELOCITY_LOOP_P, float >{ 0.f },
        cfg_reg< CONTROL_VELOCITY_LOOP_I, float >{ 0.f },
        cfg_reg< CONTROL_VELOCITY_LOOP_D, float >{ 0.f },
        cfg_reg< CONTROL_VELOCITY_LIM_MIN, float >{ 0.f },
        cfg_reg< CONTROL_VELOCITY_LIM_MAX, float >{ 0.f },
        cfg_reg< CONTROL_POSITION_LOOP_P, float >{ 0.f },
        cfg_reg< CONTROL_POSITION_LOOP_I, float >{ 0.f },
        cfg_reg< CONTROL_POSITION_LOOP_D, float >{ 0.f },
        cfg_reg< CONTROL_POSITION_LIM_MIN, float >{ 0.f },
        cfg_reg< CONTROL_POSITION_LIM_MAX, float >{ 0.f },
        cfg_reg< MINIMUM_VOLTAGE, float >{ 0.f },
        cfg_reg< MAXIMUM_TEMPERATURE, float >{ 0.f },
    };
}

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

    fw::setup_standard_callbacks( *hbridge_ptr, *acquisition_ptr, cor.ctl, cor.met );

    fw::cfg_dispatcher cfg_dis{ cfg, *acquisition_ptr, cor.ctl, cor.mon };
    cfg_dis.full_apply();
    brd::apply_config( cfg_dis );

    fw::multistart( *leds_ptr, *acquisition_ptr, *hbridge_ptr, *comms_ptr, *debug_comms_ptr );

    cor.ind.on_event( fw::ticks_ms(), indication_event::INITIALIZED );

    fw::dispatcher dis{ *comms_ptr, *acquisition_ptr, cor.ctl, cfg_dis, fw::ticks_ms() };

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

        cor.mon.tick( fw::ticks_ms() );
        cor.ind.tick( fw::ticks_ms() );
        leds_ptr->update( cor.ind.get_state() );
    }
}
