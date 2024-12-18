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
        metrics( microseconds time, float position, limits< float > position_range );

        void set_position_range( limits< float > position_range );
        void set_moving_step( float step );

        void position_irq( microseconds now, float position );

        bool is_moving() const;

        float get_position() const;

        float get_velocity() const;

private:
        microseconds       last_time_;
        mtr::posvel_kalman pv_kal_;
        static_detector    st_dec_;
};

inline float metrics::get_position() const
{
        return pv_kal_.get_position();
}

inline float metrics::get_velocity() const
{
        return pv_kal_.get_velocity();
}

inline bool metrics::is_moving() const
{
        return !st_dec_.is_static;
}

}  // namespace servio::mtr
