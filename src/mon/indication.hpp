#include "../base/base.hpp"
#include "./effects.hpp"

#pragma once

namespace servio::mon
{

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
        VOLTAGE_LOW      = 0,
        TEMPERATURE_HIGH = 1,
        HEARTBEAT        = 2,
        ENGAGED          = 3,
        DISENGAGED       = 4,
        STUCK            = 5,  // can't detect this yet
        BOOTING          = 6,
        INITIALIZED      = 7,
        INCOMING_MESSAGE = 8
};

class indication
{
public:
        indication( microseconds now )
          : last_tick_( now )
          , yellow_engaged_until_( 0_us )
          , blue_disengage_at_( 0_us )
        {
        }

        bool on_event( microseconds now, indication_event const& e );

        void tick( microseconds now );

        leds_vals const& get_state() const
        {
                return state_;
        }

private:
        leds_vals    state_;
        microseconds last_tick_;

        blinker red_bl_;
        pulser  green_pu_;

        microseconds yellow_engaged_until_;

        microseconds blue_disengage_at_;
};

}  // namespace servio::mon
