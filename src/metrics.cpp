#include "metrics.hpp"

metrics::metrics( std::chrono::milliseconds time, float position )
  : last_time_( time )
  , H_( kalman::get_observation_model() )
  , R_( kalman::get_observation_noise_covariance( observation_deviation_ ) )
  , angle_( position )
{
    kalman::angle( x_ ) = position;
}

void metrics::position_irq( std::chrono::milliseconds now, float position )
{
    if ( now == last_time_ ) {
        return;
    }

    std::chrono::milliseconds tdiff = now - last_time_;

    auto sdiff = std::chrono::duration_cast< sec_time >( tdiff );

    // TODO: this is kinda non ideal, the sdiff should be stable in the system and computed only
    // once
    auto F = kalman::get_transition_model( sdiff );
    auto B = kalman::get_control_input_model( sdiff );
    auto Q = kalman::get_process_noise_covariance( sdiff, process_deviation_ );

    kalman::control_input u{};
    kalman::angle( u ) = 0.f;
    kalman::observation z;
    kalman::angle( z ) = kalman::angle_mod( position + offset_ );

    std::tie( x_, P_ ) = kalman::predict( x_, P_, u, F, B, Q );
    std::tie( x_, P_ ) = kalman::update( x_, P_, z, H_, R_ );

    if ( kalman::requires_offset( kalman::angle( x_ ) ) ) {
        offset_ = kalman::angle_mod( offset_ + kalman::PI );
        kalman::modify_angle( x_, kalman::PI );
    }

    angle_ = kalman::angle_mod( kalman::angle( x_ ) - offset_ );

    last_time_ = now;
}
