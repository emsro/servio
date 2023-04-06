
#pragma once

struct static_detector
{
        float tolerance = 0.05f;
        float last_pos;
        bool  is_static = true;

        static_detector( float pos )
          : last_pos( pos )
        {
        }

        void update( float position )
        {
                // TODO: this won't work around 360<->0 wobble
                if ( std::abs( position - last_pos ) < tolerance ) {
                        is_static = true;
                        return;
                }
                is_static = false;
                last_pos  = position;
        }
};
