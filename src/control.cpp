#include "control.hpp"

#include <emlabcpp/match.h>
#include <emlabcpp/protocol/register_handler.h>

control::control( std::chrono::milliseconds now )
  : position_lims_( -10.f, 10.f )
  , position_pid_( now, { -10.f, 10.f } )
  , velocity_pid_( now, { -10.f, 10.f } )
  , current_pid_(
        now,
        { std::numeric_limits< int16_t >::lowest(), std::numeric_limits< int16_t >::max() } )
{
    set_pid( control_loop::POSITION, 1.f, 0.f, 0.f );
    set_pid( control_loop::VELOCITY, 0.1f, 0.01f, 0.f );
    set_pid( control_loop::CURRENT, 1024 * 32, 1024 * 4, 0 );
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

void control::set_pid( control_loop cl, float p, float i, float d )
{
    ref_module( cl ).set_pid( p, i, d );
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

control_mode control::get_mode()
{
    return state_;
}

void control::switch_to_power_control( int16_t power )
{
    state_ = control_mode::POWER;
    power_ = power;
}
void control::switch_to_current_control( std::chrono::milliseconds now, float current )
{
    if ( current >= 0.f ) {
        position_pid_.set_output( infty );
        velocity_pid_.set_momentary_limit( { -infty, current } );
    } else {
        position_pid_.set_output( -infty );
        velocity_pid_.set_momentary_limit( { current, infty } );
    }
    state_ = control_mode::CURRENT;
}
void control::switch_to_velocity_control( std::chrono::milliseconds, float velocity )
{
    state_ = control_mode::VELOCITY;
    velocity_pid_.reset_momentary_limit();
    position_pid_.set_output( velocity );
}
void control::switch_to_position_control( std::chrono::milliseconds, float position )
{
    state_ = control_mode::POSITION;
    velocity_pid_.reset_momentary_limit();
    goal_position_ = position;
}

void control::position_irq( std::chrono::milliseconds now, float position )
{
    float coeff             = 1.f;
    auto [pos_min, pos_max] = position_lims_;
    position_pid_.set_momentary_limit(
        { coeff * ( -position + pos_min ), coeff * ( -position + pos_max ) } );

    if ( state_ == control_mode::POSITION ) {
        position_pid_.update( now, position, goal_position_ );
    }
}

void control::velocity_irq( std::chrono::milliseconds now, float velocity )
{
    if ( state_ == control_mode::POWER ) {
        return;
    }

    velocity_pid_.update( now, velocity, position_pid_.get_output() );
}

void control::current_irq( std::chrono::milliseconds now, float current )
{
    if ( state_ == control_mode::POWER ) {
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
