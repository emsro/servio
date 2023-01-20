#include "indication.hpp"

bool indication::on_event( std::chrono::milliseconds now, const indication_event& e )
{
    switch ( e ) {
        case indication_event::VOLTAGE_LOW:
            if ( red_events_.full() ) {
                return false;
            }
            red_events_.push_back( 100ms );
            break;
        case indication_event::TEMPERATURE_HIGH:
            if ( red_events_.full() ) {
                return false;
            }
            red_events_.push_back( 200ms );
            break;
        case indication_event::HEARTBEAT:
            green_step_ += 0.1f;
            break;
        case indication_event::ENGAGED:
            green_offset_ = 0.5f;
            break;
        case indication_event::DISENGAGED:
            green_offset_ = 0.f;
            break;
        case indication_event::STUCK:
            yellow_engaged_until_ = now + 50ms;
            break;
        case indication_event::BOOTING:
            state_.blue = true;
            break;
        case indication_event::INITIALIZED:
            state_.blue = false;
            break;
        case indication_event::INCOMING_MESSAGE:
            state_.blue        = true;
            blue_disengage_at_ = now + 1ms;
            break;
    }
    return true;
}

void indication::tick( std::chrono::milliseconds now )
{
    tick_red( now );

    green_i_        = std::fmod( green_i_ + green_step_, 2 * pi );
    float green_val = ( std::sin( green_i_ ) + 1.f ) / 2.f + green_offset_;
    state_.green =
        static_cast< uint8_t >( em::map_range< float, float >( green_val, 0.f, 2.f, 0.f, 255.f ) );

    green_step_ = 0.f;

    state_.yellow = now < yellow_engaged_until_ ? 255u : 0u;

    if ( now > blue_disengage_at_ ) {
        state_.blue = false;
    }
}

void indication::tick_red( std::chrono::milliseconds now )
{
    if ( now < red_phase_ ) {
        state_.red = false;
        return;
    }

    if ( red_events_.empty() ) {
        red_phase_ += red_window_;
        return;
    }

    if ( red_events_.front() > now ) {
        red_events_.pop_front();
        state_.red = false;
        red_phase_ += red_window_;
        return;
    }

    std::chrono::milliseconds x = now % 100ms;

    state_.red = x < 50ms;
}
