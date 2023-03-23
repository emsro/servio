#include "control.hpp"

#include <emlabcpp/match.h>
#include <emlabcpp/protocol/register_handler.h>

control::control( microseconds now, ctl::config cfg )
  : position_lims_( cfg.position_limits )
  , position_pid_( now, cfg.position_pid, cfg.velocity_limits )
  , velocity_pid_( now, cfg.velocity_pid, cfg.current_limits )
  , current_pid_(
        now,
        cfg.current_pid,
        { std::numeric_limits< int16_t >::lowest(), std::numeric_limits< int16_t >::max() } )
{
}

ctl::pid_module& control::ref_module( control_loop cl )
{
        if ( cl == control_loop::CURRENT ) {
                return current_pid_;
        } else if ( cl == control_loop::VELOCITY ) {
                return velocity_pid_;
        } else {
                return position_pid_;
        }
}

void control::set_pid( control_loop cl, ctl::pid_coefficients coeffs )
{
        ref_module( cl ).set_pid( coeffs );
}
void control::set_limits( control_loop cl, limits< float > lim )
{
        if ( cl == control_loop::CURRENT ) {
                velocity_pid_.set_config_limit( lim );
        } else if ( cl == control_loop::VELOCITY ) {
                position_pid_.set_config_limit( lim );
        } else {
                position_lims_ = lim;
        }
}

void control::disengage()
{
        switch_to_power_control( 0 );
        engaged_ = false;
}

void control::switch_to_power_control( int16_t power )
{
        state_   = control_mode::POWER;
        power_   = power;
        engaged_ = true;
}
void control::switch_to_current_control( microseconds, float current )
{
        if ( current >= 0.f ) {
                position_pid_.set_output( infty );
                velocity_pid_.set_momentary_limit( { -infty, current } );
        } else {
                position_pid_.set_output( -infty );
                velocity_pid_.set_momentary_limit( { current, infty } );
        }
        state_   = control_mode::CURRENT;
        engaged_ = true;
}
void control::switch_to_velocity_control( microseconds, float velocity )
{
        state_ = control_mode::VELOCITY;
        velocity_pid_.reset_momentary_limit();
        position_pid_.set_output( velocity );
        engaged_ = true;
}
void control::switch_to_position_control( microseconds, float position )
{
        state_ = control_mode::POSITION;
        velocity_pid_.reset_momentary_limit();
        goal_position_ = position;
        engaged_       = true;
}

void control::position_irq( microseconds now, float position )
{
        float coeff             = 5.f;
        auto [pos_min, pos_max] = position_lims_;
        position_pid_.set_momentary_limit(
            { coeff * ( -position + pos_min ), coeff * ( -position + pos_max ) } );

        if ( state_ == control_mode::POSITION ) {
                position_pid_.update( now, position, goal_position_ );
        } else {
                position_pid_.reset( now, position );
        }
}

void control::velocity_irq( microseconds now, float velocity )
{
        if ( state_ == control_mode::POWER ) {
                velocity_pid_.reset( now, velocity );
                return;
        }
        velocity_pid_.update( now, velocity, position_pid_.get_output() );
}

void control::current_irq( microseconds now, float current )
{
        if ( state_ == control_mode::POWER ) {
                current_pid_.reset( now, current );
                return;
        }

        float fpower = current_pid_.update( now, current, velocity_pid_.get_output() );
        power_       = static_cast< int16_t >( fpower );
}

int16_t control::get_power() const
{
        return power_;
}
float control::get_desired_current() const
{
        return current_pid_.get_desired();
}
float control::get_desired_velocity() const
{
        return velocity_pid_.get_desired();
}
float control::get_desired_position() const
{
        return position_pid_.get_desired();
}
