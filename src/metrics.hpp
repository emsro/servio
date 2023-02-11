#include "base.hpp"
#include "kalman.hpp"

#include <array>
#include <tuple>

#pragma once

class metrics
{
public:
        metrics( std::chrono::milliseconds time, float position );

        void  position_irq( std::chrono::milliseconds now, float position );
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
        float observation_deviation_ = 0.01f;
        float process_deviation_     = 0.005f;

        std::chrono::milliseconds last_time_;

        kalman::observation_model            H_;
        kalman::observation_noise_covariance R_;

        float                    angle_;
        float                    offset_ = 0.f;
        kalman::state            x_;
        kalman::state_covariance P_;
};
