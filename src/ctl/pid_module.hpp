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
          : pid_( now.count(), { coeffs, limits } )
        {
                set_config_limit( limits );
        }

        void set_pid( pid_coefficients coeff )
        {
                pid_.cfg.coefficients = coeff;
        }

        void set_config_limit( limits< float > lim )
        {
                em::update_limits( pid_, lim );
        }

        float get_output() const
        {
                return pid_.output;
        }

        void set_output( float val )
        {
                em::update_output( pid_, val );
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

        float update( microseconds now, float measured )
        {
                return em::update( pid_, now.count(), measured, goal_ );
        }

        void set_goal( float g )
        {
                goal_ = g;
        }

        float get_goal() const
        {
                return goal_;
        }

        float get_measured() const
        {
                return pid_.last_measured;
        }

private:
        float goal_ = 0.F;
        pid   pid_;
};

}  // namespace ctl
