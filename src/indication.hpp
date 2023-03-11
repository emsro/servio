#include "base.hpp"

#include <emlabcpp/static_circular_buffer.h>

#pragma once

// possible events:
// - system stops itself due to un-expected error - red glows
// - voltage too low - red blinks with 100ms interval for 100ms every 1s
// - temperature too high - red blinks with 100ms interval for 200ms every 1s
//
// - heartbeat - pulsing of green led - heartbeat controls frequency of the pulses
// - engaged/disengaged - pulsing of green led - eng/diseng controls intesinty of pulses
//
// - unable to reach goal - yellow is ON for 50ms
//
// - booting - blue is ON
// - initialized - blue is OFF
// - for every incoming message - blue is ON for 1ms

enum class indication_event
{
        //    ERROR,
        VOLTAGE_LOW,
        TEMPERATURE_HIGH,
        HEARTBEAT,
        ENGAGED,
        DISENGAGED,
        STUCK,  // can't detect this yet
        BOOTING,
        INITIALIZED,
        INCOMING_MESSAGE
};

class indication
{
public:
        indication( microseconds now )
          : last_tick_( now )
          , red_phase_( now )
          , yellow_engaged_until_( 0_us )
          , blue_disengage_at_( 0_us )
        {
        }

        bool on_event( microseconds now, const indication_event& e );

        void tick( microseconds now );

        const leds_vals& get_state() const
        {
                return state_;
        }

private:
        void tick_red( microseconds now );

        leds_vals    state_;
        microseconds last_tick_;

        static constexpr microseconds                 red_window_ = 5_s;
        microseconds                                  red_phase_;
        em::static_circular_buffer< microseconds, 2 > red_events_;

        float green_i_      = 0.f;
        float green_step_   = 0.f;
        float green_offset_ = 0.f;

        microseconds yellow_engaged_until_;

        microseconds blue_disengage_at_;
};
