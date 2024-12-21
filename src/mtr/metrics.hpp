#include "../base/base.hpp"
#include "./posvel_kalman.hpp"
#include "./static_detector.hpp"

#include <array>
#include <tuple>

#pragma once

namespace servio::mtr
{

class metrics
{
public:
        metrics(
            microseconds    time,
            float           position,
            float           velocity,
            limits< float > position_range );

        void set_position_range( limits< float > position_range );
        void set_moving_step( float step );

        void position_irq( microseconds now, float position );

        bool is_moving() const
        {
                return !st_dec_.is_static;
        }

        float get_position() const
        {
                return pv_kal_.get_position();
        }

        float get_velocity() const
        {
                return pv_kal_.get_velocity();
        }

private:
        microseconds       last_time_;
        mtr::posvel_kalman pv_kal_;
        static_detector    st_dec_;
};

}  // namespace servio::mtr
