#include "base.hpp"
#include "ctl/config.hpp"
#include "ctl/linear_transition_regulator.hpp"
#include "ctl/pid_module.hpp"

#include <variant>

#pragma once

class control
{

public:
        control( microseconds, ctl::config cfg );

        void set_pid( control_loop, ctl::pid_coefficients coeffs );
        void set_limits( control_loop, limits< float > lim );
        void set_static_friction( float scale, float decay );

        [[nodiscard]] control_mode get_mode() const
        {
                return state_;
        }

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

private:
        ctl::pid_module& ref_module( control_loop );

        control_mode state_ = control_mode::POWER;

        float goal_position_ = 0.F;

        limits< float > position_lims_;

        ctl::pid_module position_pid_;
        ctl::pid_module velocity_pid_;

        ctl::linear_transition_regulator current_scale_regl_;

        ctl::pid_module current_pid_;

        int16_t power_ = 0;
};
