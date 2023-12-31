#include "control.hpp"

#include "base/base.hpp"

#include <emlabcpp/match.h>
#include <emlabcpp/protocol/register_handler.h>

namespace servio::ctl
{

control::control( base::microseconds now, ctl::config cfg )
  : position_lims_( cfg.position_limits )
  , position_pid_( now.count(), { cfg.position_pid, cfg.current_limits } )
  , velocity_pid_( now.count(), { cfg.velocity_pid, cfg.current_limits } )
  , current_scale_regl_{ .low_point = 1.F, .high_point = 2.F, .last_time = now }
  , current_pid_(
        now.count(),
        { cfg.current_pid,
          base::limits< float >{
              std::numeric_limits< int16_t >::lowest(),
              std::numeric_limits< int16_t >::max() } } )
{
        set_static_friction( cfg.static_friction_scale, cfg.static_friction_decay );
}

ctl::pid& control::ref_module( base::control_loop cl )
{
        if ( cl == base::control_loop::CURRENT ) {
                return current_pid_;
        }
        if ( cl == base::control_loop::VELOCITY ) {
                return velocity_pid_;
        }
        return position_pid_;
}

void control::set_pid( base::control_loop cl, ctl::pid_coefficients coeffs )
{
        ref_module( cl ).cfg.coefficients = coeffs;
}
void control::set_limits( base::control_loop cl, base::limits< float > lim )
{
        switch ( cl ) {
        case base::control_loop::CURRENT:
                current_lims_.config_lims = lim;
                em::update_limits( velocity_pid_, lim );
                em::update_limits( position_pid_, lim );
                break;
        case base::control_loop::VELOCITY:
                velocity_lims_.config_lims = lim;
                break;
        case base::control_loop::POSITION:
                position_lims_ = lim;
                break;
        }
}

void control::set_pos_to_vel_lim_scale( float scale )
{
        position_to_velocity_lim_scale_ = scale;
}

void control::set_vel_to_curr_lim_scale( float scale )
{
        velocity_to_current_lim_scale_ = scale;
}

void control::set_static_friction( float scale, float decay )
{
        current_scale_regl_.set_config( scale, decay );
}

void control::disengage()
{
        state_ = base::control_mode::DISENGAGED;
        power_ = 0;
}

void control::switch_to_power_control( int16_t power )
{
        state_ = base::control_mode::POWER;
        power_ = power;
}
void control::switch_to_current_control( base::microseconds, float current )
{
        state_        = base::control_mode::CURRENT;
        current_goal_ = clamp( current, current_lims_.config_lims );
}
void control::switch_to_velocity_control( base::microseconds, float velocity )
{
        state_              = base::control_mode::VELOCITY;
        velocity_goal_      = clamp( velocity, velocity_lims_.config_lims );
        velocity_pid_.i_sum = 0;
}
void control::switch_to_position_control( base::microseconds, float position )
{
        state_              = base::control_mode::POSITION;
        position_goal_      = clamp( position, position_lims_ );
        position_pid_.i_sum = 0;
}

void control::moving_irq( base::microseconds now, bool is_moving )
{
        current_scale_regl_.update( now, is_moving );
}

[[gnu::flatten]] void control::position_irq( base::microseconds now, float position )
{
        base::limits< float >& vlims = velocity_lims_.pos_derived_lims;

        vlims.max() = position_to_velocity_lim_scale_ * ( position_lims_.max() - position );
        vlims.min() = position_to_velocity_lim_scale_ * ( position_lims_.min() - position );

        if ( state_ == base::control_mode::POSITION ) {
                em::update( position_pid_, now.count(), position, position_goal_ );
        } else {
                em::reset( position_pid_, now.count(), position );
        }
}

[[gnu::flatten]] void control::velocity_irq( base::microseconds now, float velocity )
{
        base::limits< float >  vlims = get_velocity_limits();
        base::limits< float >& clims = current_lims_.vel_derived_lims;
        clims.max()                  = velocity_to_current_lim_scale_ * ( vlims.max() - velocity );
        clims.min()                  = velocity_to_current_lim_scale_ * ( vlims.min() - velocity );

        if ( state_ == base::control_mode::VELOCITY ) {
                float goal = clamp( velocity_goal_, vlims );
                em::update( velocity_pid_, now.count(), velocity, goal );
        } else {
                em::reset( velocity_pid_, now.count(), velocity );
        }
}

[[gnu::flatten]] void control::current_irq( base::microseconds now, float current )
{
        if ( state_ == base::control_mode::DISENGAGED || state_ == base::control_mode::POWER ) {
                em::reset( current_pid_, now.count(), current );
                return;
        }

        float desired_curr = get_desired_current();

        const base::limits< float > lims = get_current_limits();
        desired_curr                     = clamp( desired_curr, lims );

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
        case base::control_mode::POWER:
        case base::control_mode::DISENGAGED:
                break;
        case base::control_mode::CURRENT:
                return current_goal_;
        case base::control_mode::VELOCITY:
                return velocity_pid_.output * current_scale_regl_.state;
        case base::control_mode::POSITION:
                return position_pid_.output * current_scale_regl_.state;
        }
        return 0.F;
}

float control::get_desired_velocity() const
{
        return velocity_goal_;
}

float control::get_desired_position() const
{
        return position_goal_;
}

base::limits< float > control::get_current_limits() const
{
        return em::intersection( current_lims_.config_lims, current_lims_.vel_derived_lims );
}

base::limits< float > control::get_velocity_limits() const
{
        return em::intersection( velocity_lims_.config_lims, velocity_lims_.pos_derived_lims );
}

}  // namespace servio::ctl
