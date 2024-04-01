#include "base/base.hpp"

#include <algorithm>

#pragma once

namespace servio::ctl
{

struct linear_transition_regulator
{
        float low_point  = 1.F;
        float high_point = 2.F;

        float        decay = 1.F;
        microseconds last_time;

        float state = low_point;

        void set_config( float scale, float dec )
        {
                high_point = scale;
                decay      = dec / 1'000'000.F;  // input decay is /s, this makes it /us
        }

        void update( microseconds now, bool is_moving )
        {
                const auto  tdiff = now - last_time;
                const float dir   = is_moving ? -1.F : 1.F;
                const float step  = decay * static_cast< float >( tdiff.count() );

                state     = std::clamp( state + dir * step, low_point, high_point );
                last_time = now;
        }
};

}  // namespace servio::ctl
