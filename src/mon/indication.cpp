#include "indication.hpp"

namespace servio::mon
{

bool indication::on_event( microseconds now, indication_event const& e )
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
                // XXX: unfinished
                break;
        case indication_event::BOOTING:
                blue_disengage_at_ = now + 10_s;
                break;
        case indication_event::INITIALIZED:
                blue_disengage_at_ = now;
                break;
        case indication_event::INCOMING_MESSAGE:
                blue_disengage_at_ = now + 10_ms;
                break;
        }
        return true;
}

void indication::tick( microseconds now )
{

        state_.red = red_bl_.update( now );

        state_.green = green_pu_.update();

        state_.blue = now < blue_disengage_at_;
}

}  // namespace servio::mon
