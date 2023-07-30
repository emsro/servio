#include "indication.hpp"

#include <cmath>

bool indication::on_event( microseconds now, const indication_event& e )
{
        switch ( e ) {
        case indication_event::VOLTAGE_LOW:
                if ( red_events_.full() ) {
                        return false;
                }
                red_events_.push_back( 500_ms );
                break;
        case indication_event::TEMPERATURE_HIGH:
                if ( red_events_.full() ) {
                        return false;
                }
                red_events_.push_back( 1000_ms );
                break;
        case indication_event::HEARTBEAT:
                green_step_ += 0.1F;
                break;
        case indication_event::ENGAGED:
                green_offset_ = 0.5F;
                break;
        case indication_event::DISENGAGED:
                green_offset_ = 0.F;
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
                blue_disengage_at_ = now + 1_ms;
                break;
        }
        return true;
}

float sin_approx( float x )
{
        const float x3 = x * x * x;
        return x - x3 / 6.F + ( x3 * x * x ) / 120.F;
}

void indication::tick( microseconds now )
{
        tick_red( now );

        green_i_              = std::fmod( green_i_ + green_step_, 2 * pi );
        const float green_val = ( sin_approx( green_i_ ) + 1.F ) / 2.F + green_offset_;
        state_.green          = static_cast< uint8_t >(
            em::map_range< float, float >( green_val, 0.F, 2.F, 0.F, 255.F ) );

        green_step_ = 0.F;

        state_.yellow = now < yellow_engaged_until_ ? 255U : 0U;

        if ( now > blue_disengage_at_ ) {
                state_.blue = false;
        }
}

void indication::tick_red( microseconds now )
{
        if ( now < red_phase_ ) {
                state_.red = false;
                return;
        }

        if ( red_events_.empty() ) {
                red_phase_ += red_window;
                return;
        }

        if ( red_events_.front() > now ) {
                red_events_.pop_front();
                state_.red = false;
                red_phase_ += red_window;
                return;
        }

        const microseconds x = now % 500_ms;

        state_.red = x < 250_ms;
}
