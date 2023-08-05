#include "control.hpp"

#include <emlabcpp/match.h>
#include <emlabcpp/protocol/register_handler.h>

control::control( microseconds now, ctl::config cfg )
  : position_lims_( cfg.position_limits )
  , position_pid_( now.count(), { cfg.position_pid, cfg.current_limits } )
  , velocity_pid_( now.count(), { cfg.velocity_pid, cfg.current_limits } )
  , current_scale_regl_{ .low_point = 1.F, .high_point = 2.F, .last_time = now }
  , current_pid_(
        now.count(),
        { cfg.current_pid,
          limits< float >{
              std::numeric_limits< int16_t >::lowest(),
              std::numeric_limits< int16_t >::max() } } )
{
        set_static_friction( cfg.static_friction_scale, cfg.static_friction_decay );
}

ctl::pid& control::ref_module( control_loop cl )
{
        if ( cl == control_loop::CURRENT ) {
                return current_pid_;
        }
        if ( cl == control_loop::VELOCITY ) {
                return velocity_pid_;
        }
        return position_pid_;
}

void control::set_pid( control_loop cl, ctl::pid_coefficients coeffs )
{
        ref_module( cl ).cfg.coefficients = coeffs;
}
void control::set_limits( control_loop cl, limits< float > lim )
{
        switch ( cl ) {
        case control_loop::CURRENT:
                em::update_limits( velocity_pid_, lim );
                em::update_limits( position_pid_, lim );
                break;
        case control_loop::VELOCITY:
                velocity_lims_ = lim;
                break;
        case control_loop::POSITION:
                position_lims_ = lim;
                break;
        }
}

void control::set_static_friction( float scale, float decay )
{
        current_scale_regl_.set_config( scale, decay );
}

void control::disengage()
{
        state_ = control_mode::DISENGAGED;
        power_ = 0;
}

void control::switch_to_power_control( int16_t power )
{
        state_ = control_mode::POWER;
        power_ = power;
}
void control::switch_to_current_control( microseconds, float current )
{
        state_        = control_mode::CURRENT;
        current_goal_ = current;
}
void control::switch_to_velocity_control( microseconds, float velocity )
{
        state_         = control_mode::VELOCITY;
        velocity_goal_ = velocity;
}
void control::switch_to_position_control( microseconds, float position )
{
        state_         = control_mode::POSITION;
        position_goal_ = position;
}

void control::moving_irq( microseconds now, bool is_moving )
{
        current_scale_regl_.update( now, is_moving );
}

void control::position_irq( microseconds now, float position )
{
        if ( state_ == control_mode::POSITION ) {
                em::update( position_pid_, now.count(), position, position_goal_ );
        } else {
                em::reset( position_pid_, now.count(), position );
        }
}

void control::velocity_irq( microseconds now, float velocity )
{
        if ( state_ == control_mode::VELOCITY ) {
                em::update( velocity_pid_, now.count(), velocity, velocity_goal_ );
        } else {
                em::reset( velocity_pid_, now.count(), velocity );
        }
}

void control::current_irq( microseconds now, float current )
{
        if ( state_ == control_mode::DISENGAGED || state_ == control_mode::POWER ) {
                em::reset( current_pid_, now.count(), current );
                return;
        }

        float desired_curr = get_desired_current();

        limits< float > lims = get_current_limits();
        desired_curr         = clamp( desired_curr, lims );

        const float fpower = em::update( current_pid_, now.count(), current, desired_curr );
        power_             = static_cast< int16_t >( fpower );
}

int16_t control::get_power() const
{
        return power_;
}

float control::get_desired_current() const
{
        switch ( state_ ) {
        case control_mode::POWER:
        case control_mode::DISENGAGED:
                break;
        case control_mode::CURRENT:
                return current_goal_;
        case control_mode::VELOCITY:
                return velocity_pid_.output * current_scale_regl_.state;
        case control_mode::POSITION:
                return position_pid_.output * current_scale_regl_.state;
        }
        return 0.f;
}

float control::get_desired_velocity() const
{
        return velocity_goal_;
}

float control::get_desired_position() const
{
        return position_goal_;
}

limits< float > control::get_current_limits() const
{
        return em::intersection(
            current_lims_.config_lims,
            current_lims_.pos_derived_lims,
            current_lims_.vel_derived_lims );
}
