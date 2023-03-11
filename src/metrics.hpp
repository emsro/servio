#include "base.hpp"
#include "kalman.hpp"

#include <array>
#include <tuple>

#pragma once

class metrics
{
public:
        metrics( microseconds time, float position, limits< float > position_range );

        void set_position_range( limits< float > position_range );

        void position_irq( microseconds now, float position );

        float get_position() const
        {
                return angle_;
        }

        float get_velocity() const
        {
                return kalman::velocity( x_ );
        }

private:
        // TODO: hardcoded constant is meh
        float observation_deviation_ = 0.000005f;
        float process_deviation_     = 0.005f;

        microseconds last_time_;

        kalman::observation_model            H_;
        kalman::observation_noise_covariance R_;

        float                    angle_;
        kalman::state_range      sr_;
        float                    offset_ = 0.f;
        kalman::state            x_;
        kalman::state_covariance P_;
};
