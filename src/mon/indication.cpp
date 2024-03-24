#include "indication.hpp"

namespace servio::mon
{

bool indication::on_event( base::microseconds now, const indication_event& e )
{
        switch ( e ) {
        case indication_event::VOLTAGE_LOW:
                red_bl_.state[0] = true;
                break;
        case indication_event::TEMPERATURE_HIGH:
                red_bl_.state[1] = true;
                red_bl_.state[2] = true;
                break;
        case indication_event::HEARTBEAT:
                green_pu_.val += 0.00001F;
                break;
        case indication_event::ENGAGED:
                green_pu_.intensity = 1.0F;
                break;
        case indication_event::DISENGAGED:
                green_pu_.intensity = 0.5F;
                break;
        case indication_event::STUCK:
                yellow_engaged_until_ = now + 50_ms;
                break;
        case indication_event::BOOTING:
                state_.blue = true;
                break;
        case indication_event::INITIALIZED:
                state_.blue = false;
                break;
        case indication_event::INCOMING_MESSAGE:
                state_.blue        = true;
                blue_disengage_at_ = now + 10_ms;
                break;
        }
        return true;
}

void indication::tick( base::microseconds now )
{

        state_.red = red_bl_.update( now );

        state_.green = green_pu_.update();

        state_.yellow = now < yellow_engaged_until_ ? 255U : 0U;

        if ( now > blue_disengage_at_ )
                state_.blue = false;
}

}  // namespace servio::mon
