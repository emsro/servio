#include <cmath>

#pragma once

namespace servio
{

struct static_detector
{
        float step = 0.05F;
        float last_pos;
        bool  is_static = true;

        static_detector( float pos )
          : last_pos( pos )
        {
        }

        void set_step( float stp )
        {
                step = stp;
        }

        void update( float position )
        {
                // TODO: this won't work around 360<->0 wobble
                if ( std::abs( position - last_pos ) < step ) {
                        is_static = true;
                        return;
                }
                is_static = false;
                last_pos  = position;
        }
};

}  // namespace servio
