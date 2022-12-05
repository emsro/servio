#include "control.hpp"

#include <emlabcpp/match.h>
#include <emlabcpp/protocol/register_handler.h>

control::control( std::chrono::milliseconds now )
  : position_pid_( now.count() )
  , velocity_pid_( now.count() )
  , current_pid_( now.count() )
{
    update_config();
}

void control::update_config()
{
    set_current_pid_config( pidconf{
        .p   = 1024 * 32,
        .i   = 1024 * 4,
        .d   = 0,
        .min = std::numeric_limits< int16_t >::lowest(),
        .max = std::numeric_limits< int16_t >::max() } );
    set_velocity_pid_config( pidconf{ .p = 0.1f, .i = 0.01f, .d = 0, .min = -10, .max = 10 } );
    set_position_pid_config( pidconf{ .p = 1, .i = 0, .d = 0, .min = -10, .max = 10 } );
}

control_mode control::get_mode()
{
    return em::match(
        state_,
        [&]( const power_control& ) {
            return control_mode::POWER;
        },
        [&]( const current_control& ) {
            return control_mode::CURRENT;
        },
        [&]( const velocity_control& ) {
            return control_mode::VELOCITY;
        },
        [&]( const position_control& ) {
            return control_mode::POSITION;
        } );
}

void control::set_current_pid_config( const pidconf& conf )
{
    current_pid_.set_config( conf );
}

void control::set_velocity_pid_config( const pidconf& conf )
{
    velocity_pid_.set_config( conf );
}

void control::set_position_pid_config( const pidconf& conf )
{
    position_pid_.set_config( conf );
}

void control::switch_to_power_control( int16_t power )
{
    state_.emplace< power_control >( power );
    power_ = power;
}
void control::switch_to_current_control( std::chrono::milliseconds now, float current )
{
    state_.emplace< current_control >( current );
}
void control::switch_to_velocity_control( std::chrono::milliseconds now, float velocity )
{
    state_.emplace< velocity_control >( velocity, current_pid_.get_desired() );
}
void control::switch_to_position_control( std::chrono::milliseconds now, float position )
{
    state_.emplace< position_control >(
        position, velocity_pid_.get_desired(), current_pid_.get_desired() );
}

void control::position_irq( std::chrono::milliseconds now, float position )
{
    em::match(
        state_,
        [&]( const power_control& ) {},
        [&]( const current_control& ) {},
        [&]( const velocity_control& ) {},
        [&]( position_control& pc ) {
            pc.velocity = position_pid_.update( now.count(), position, pc.position );
        } );
}

void control::velocity_irq( std::chrono::milliseconds now, float velocity )
{
    em::match(
        state_,
        [&]( const power_control& ) {},
        [&]( const current_control& ) {},
        [&]( velocity_control& vc ) {
            vc.current = velocity_pid_.update( now.count(), velocity, vc.velocity );
        },
        [&]( position_control& pc ) {
            pc.current = velocity_pid_.update( now.count(), velocity, pc.velocity );
        } );
}

void control::current_irq( std::chrono::milliseconds now, float current )
{
    std::optional< float > goal_current;
    em::match(
        state_,
        [&]( const power_control& ) {},
        [&]( const current_control& cc ) {
            goal_current = cc.current;
        },
        [&]( const velocity_control& vc ) {
            goal_current = vc.current;
        },
        [&]( const position_control& pc ) {
            goal_current = pc.current;
        } );

    if ( goal_current ) {
        float fpower = current_pid_.update( now.count(), current, *goal_current );
        power_       = static_cast< int16_t >( fpower );
    }
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
