#include "base.hpp"
#include "ctl/config.hpp"
#include "ctl/pid_module.hpp"

#include <variant>

#pragma once

class control
{

public:
        control( microseconds, ctl::config cfg );

        void set_pid( control_loop, ctl::pid_coefficients coeffs );
        void set_limits( control_loop, limits< float > lim );

        bool is_engaged() const
        {
                return engaged_;
        }
        control_mode get_mode() const
        {
                return state_;
        }

        void disengage();

        void switch_to_power_control( int16_t power );
        void switch_to_current_control( microseconds now, float current );
        void switch_to_velocity_control( microseconds now, float velocity );
        void switch_to_position_control( microseconds now, float position );

        void moving_irq( bool is_moving );

        void position_irq( microseconds now, float position );
        void velocity_irq( microseconds now, float velocity );
        void current_irq( microseconds now, float current );

        int16_t get_power() const;

        float get_desired_current() const;
        float get_desired_velocity() const;
        float get_desired_position() const;

private:
        ctl::pid_module& ref_module( control_loop );

        bool         engaged_ = false;
        control_mode state_   = control_mode::POWER;

        float goal_position_ = 0.f;

        limits< float > position_lims_;

        ctl::pid_module position_pid_;
        ctl::pid_module velocity_pid_;

        float current_scale_ = 1.f;

        ctl::pid_module current_pid_;

        int16_t power_ = 0;
};
