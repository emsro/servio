
#include "base.hpp"

#include <emlabcpp/pid.h>

#pragma once

namespace ctl
{

using pid      = em::pid< typename std::chrono::milliseconds::rep >;
using pid_conf = typename pid::config;

class pid_module
{
public:
    pid_module( std::chrono::milliseconds now, limits< float > lim )
      : momentary_lim_( lim )
      , config_lim_( lim )
      , pid_( now.count() )
    {
        update_limit();
    }

    void set_pid( float p, float i, float d )
    {
        pid_.set_pid( p, i, d );
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
        return { pid_.get_config().min, pid_.get_config().max };
    }

    float get_desired() const
    {
        return pid_.get_desired();
    }

    float update( std::chrono::milliseconds now, float measured, float goal )
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
