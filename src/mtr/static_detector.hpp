#include "../base.hpp"

#include <algorithm>
#include <cmath>

#pragma once

namespace servio::mtr
{

struct static_detector
{
        float        step   = 0.05F;
        microseconds window = 200_ms;
        float        range  = 2 * pi;
        float        last_pos;
        microseconds last_time;
        bool         is_static = true;

        static_detector( microseconds now, float pos )
          : last_pos( pos )
          , last_time( now )
        {
        }

        void set_step( float stp )
        {
                step = stp;
        }

        void set_window( microseconds w )
        {
                window = w;
        }

        void set_range( float r )
        {
                range = r;
        }

        void update( microseconds now, float position )
        {
                float const d = std::abs( position - last_pos );
                if ( std::min( d, range - d ) >= step ) {
                        is_static = false;
                        last_pos  = position;
                        last_time = now;
                } else if ( now - last_time >= window ) {
                        is_static = true;
                }
        }
};

}  // namespace servio::mtr
