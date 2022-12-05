#include "base.hpp"

#include <emlabcpp/pid.h>
#include <variant>

#pragma once

class control
{
    using pid = em::pid< typename std::chrono::milliseconds::rep >;

    struct power_control
    {
        const int16_t power;

        power_control( int16_t pwr )
          : power( pwr )
        {
        }
    };

    struct current_control
    {
        const float current;

        current_control( float c )
          : current( c )
        {
        }
    };

    struct velocity_control
    {
        const float velocity;
        float       current;

        velocity_control( float vel, float cur )
          : velocity( vel )
          , current( cur )
        {
        }
    };

    struct position_control
    {
        const float position;
        float       velocity;
        float       current;

        position_control( float pos, float vel, float cur )
          : position( pos )
          , velocity( vel )
          , current( cur )
        {
        }
    };

public:
    using pidconf = typename pid::config;
    using state_variant =
        std::variant< power_control, current_control, velocity_control, position_control >;

    control( std::chrono::milliseconds );

    control_mode get_mode();

    void set_current_pid_config( const pidconf& conf );
    void set_velocity_pid_config( const pidconf& conf );
    void set_position_pid_config( const pidconf& conf );

    void switch_to_power_control( int16_t power );
    void switch_to_current_control( std::chrono::milliseconds now, float current );
    void switch_to_velocity_control( std::chrono::milliseconds now, float velocity );
    void switch_to_position_control( std::chrono::milliseconds now, float position );

    void position_irq( std::chrono::milliseconds now, float position );
    void velocity_irq( std::chrono::milliseconds now, float velocity );
    void current_irq( std::chrono::milliseconds now, float current );

    int16_t get_power() const;

    float get_desired_current() const;
    float get_desired_velocity() const;
    float get_desired_position() const;

private:
    void update_config();

    state_variant state_ = power_control{ 0 };

    pid position_pid_;

    pid velocity_pid_;

    pid current_pid_;

    int16_t power_ = 0;
};
