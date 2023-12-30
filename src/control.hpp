#include "base.hpp"
#include "ctl/config.hpp"
#include "ctl/linear_transition_regulator.hpp"

#include <emlabcpp/pid.h>
#include <limits>
#include <variant>

#pragma once

namespace servio
{

class control
{

public:
        control( microseconds, ctl::config cfg = {} );

        void set_pid( control_loop, ctl::pid_coefficients coeffs );
        void set_limits( control_loop, limits< float > lim );
        void set_static_friction( float scale, float decay );
        void set_pos_to_vel_lim_scale( float scale );
        void set_vel_to_curr_lim_scale( float scale );

        void disengage();

        void switch_to_power_control( int16_t power );
        void switch_to_current_control( microseconds now, float current );
        void switch_to_velocity_control( microseconds now, float velocity );
        void switch_to_position_control( microseconds now, float position );

        void moving_irq( microseconds now, bool is_moving );

        void position_irq( microseconds now, float position );
        void velocity_irq( microseconds now, float velocity );
        void current_irq( microseconds now, float current );

        int16_t get_power() const;

        float get_desired_current() const;
        float get_desired_velocity() const;
        float get_desired_position() const;

        [[nodiscard]] control_mode get_mode() const
        {
                return state_;
        }

private:
        limits< float > get_current_limits() const;
        limits< float > get_velocity_limits() const;
        ctl::pid&       ref_module( control_loop );

        control_mode state_ = control_mode::POWER;

        limits< float > position_lims_;
        float           position_goal_ = 0.F;
        ctl::pid        position_pid_;
        float           position_to_velocity_lim_scale_ = std::numeric_limits< float >::max();

        struct
        {
                limits< float > config_lims{ -infty, infty };
                limits< float > pos_derived_lims{ -infty, infty };
        } velocity_lims_;
        float    velocity_goal_ = 0.F;
        ctl::pid velocity_pid_;
        float    velocity_to_current_lim_scale_ = std::numeric_limits< float >::max();

        struct
        {
                limits< float > config_lims{ -infty, infty };
                limits< float > vel_derived_lims{ -infty, infty };
        } current_lims_;
        float                            current_goal_ = 0.F;
        ctl::linear_transition_regulator current_scale_regl_;
        ctl::pid                         current_pid_;

        int16_t power_ = 0;
};

}  // namespace servio
