#include "metrics.hpp"

metrics::metrics( microseconds time, float position, limits< float > position_range )
  : last_time_( time )
  , H_( kalman::get_observation_model() )
  , R_( kalman::get_observation_noise_covariance( observation_deviation_ ) )
  , angle_( position )
  , sr_{ .offset = 0.f, .size = 0.f }
{
        set_position_range( position_range );
        kalman::angle( x_ ) = kalman::angle_mod( position, sr_ );
}

void metrics::set_position_range( limits< float > position_range )
{
        sr_.offset = position_range.min;
        sr_.size   = position_range.max - position_range.min;
}

void metrics::position_irq( microseconds now, float position )
{
        if ( now == last_time_ ) {
                return;
        }

        microseconds tdiff = now - last_time_;

        auto sdiff = std::chrono::duration_cast< sec_time >( tdiff );

        // TODO: this is kinda non ideal, the sdiff should be stable in the system and computed only
        // once
        auto F = kalman::get_transition_model( sdiff );
        auto B = kalman::get_control_input_model( sdiff );
        auto Q = kalman::get_process_noise_covariance( sdiff, process_deviation_ );

        kalman::control_input u{};
        u[0][0] = 0.f;
        kalman::observation z;
        kalman::angle( z ) = kalman::angle_mod( position + offset_, sr_ );

        std::tie( x_, P_ ) = kalman::predict( x_, P_, u, F, B, Q );
        std::tie( x_, P_ ) = kalman::update( x_, P_, z, H_, R_ );

        if ( kalman::requires_offset( kalman::angle( x_ ) ) ) {
                offset_ = kalman::angle_mod( offset_ + pi, sr_ );
                kalman::modify_angle( x_, pi, sr_ );
        }

        angle_     = kalman::angle_mod( kalman::angle( x_ ) - offset_, sr_ );
        last_time_ = now;
}
