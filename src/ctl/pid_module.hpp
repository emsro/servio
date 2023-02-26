
#include "base.hpp"

#include <emlabcpp/pid.h>

#pragma once

namespace ctl
{

using pid              = em::pid< typename std::chrono::microseconds::rep >;
using pid_conf         = typename pid::config;
using pid_coefficients = em::pid_coefficients;

class pid_module
{
public:
        pid_module( std::chrono::microseconds now, pid_coefficients coeffs, limits< float > limits )
          : momentary_lim_( limits )
          , config_lim_( limits )
          , pid_( now.count(), { coeffs, limits } )
        {
                update_limit();
        }

        void set_pid( pid_coefficients coeff )
        {
                pid_.set_pid( coeff );
        }

        void set_config_limit( limits< float > lim )
        {
                config_lim_ = lim;
                update_limit();
        }

        void set_momentary_limit( limits< float > lim )
        {
                momentary_lim_ = lim;
                update_limit();
        }

        float get_output() const
        {
                return pid_.get_output();
        }

        void set_output( float val )
        {
                pid_.set_output( val );
        }

        void reset_momentary_limit()
        {
                set_momentary_limit( { -infty, infty } );
        }

        limits< float > get_limits()
        {
                return pid_.get_limits();
        }

        float get_desired() const
        {
                return pid_.get_desired();
        }

        float update( std::chrono::microseconds now, float measured, float goal )
        {
                return pid_.update( now.count(), measured, goal );
        }

private:
        void update_limit()
        {
                pid_.set_limits(
                    std::max( momentary_lim_.min, config_lim_.min ),
                    std::min( momentary_lim_.max, config_lim_.max ) );
        }

        limits< float > momentary_lim_;
        limits< float > config_lim_;
        pid             pid_;
};

}  // namespace ctl
