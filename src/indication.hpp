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
        indication( std::chrono::microseconds now )
          : last_tick_( now )
          , red_phase_( now )
        {
        }

        bool on_event( std::chrono::microseconds now, const indication_event& e );

        void tick( std::chrono::microseconds now );

        const leds_vals& get_state() const
        {
                return state_;
        }

private:
        void tick_red( std::chrono::microseconds now );

        leds_vals                 state_;
        std::chrono::microseconds last_tick_;

        static constexpr std::chrono::microseconds                 red_window_ = 1s;
        std::chrono::microseconds                                  red_phase_;
        em::static_circular_buffer< std::chrono::microseconds, 2 > red_events_;

        float green_i_      = 0.f;
        float green_step_   = 0.f;
        float green_offset_ = 0.f;

        std::chrono::microseconds yellow_engaged_until_;

        std::chrono::microseconds blue_disengage_at_;
};
