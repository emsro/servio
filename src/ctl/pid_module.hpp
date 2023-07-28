
#include "base.hpp"

#include <emlabcpp/pid.h>

#pragma once

namespace ctl
{

using pid              = em::pid< typename microseconds::rep >;
using pid_conf         = typename pid::config;
using pid_coefficients = em::pid_coefficients;

class pid_module
{
public:
        pid_module( microseconds now, pid_coefficients coeffs, limits< float > limits )
          : momentary_lim_( limits )
          , config_lim_( limits )
          , pid_( now.count(), { coeffs, limits } )
        {
                update_limit();
        }

        void set_pid( pid_coefficients coeff )
        {
                pid_.cfg.coefficients = coeff;
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
                return pid_.output;
        }

        void set_output( float val )
        {
                em::update_output( pid_, val );
        }

        void reset_momentary_limit()
        {
                set_momentary_limit( { -infty, infty } );
        }

        limits< float > get_limits() const
        {
                return pid_.cfg.limits;
        }

        void reset( microseconds now, float val )
        {
                pid_.last_time     = now.count();
                pid_.last_measured = val;
        }

        float update( microseconds now, float measured, float goal )
        {
                last_goal_ = goal;
                return em::update( pid_, now.count(), measured, goal );
        }

        float get_desired() const
        {
                return last_goal_;
        }

        float get_measured() const
        {
                return pid_.last_measured;
        }

private:
        void update_limit()
        {
                em::update_limits(
                    pid_,
                    { std::max( momentary_lim_.min, config_lim_.min ),
                      std::min( momentary_lim_.max, config_lim_.max ) } );
        }

        float           last_goal_ = 0.F;
        limits< float > momentary_lim_;
        limits< float > config_lim_;
        pid             pid_;
};

}  // namespace ctl
