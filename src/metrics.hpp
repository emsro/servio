#include "base.hpp"
#include "mtr/posvel_kalman.hpp"
#include "static_detector.hpp"

#include <array>
#include <tuple>

#pragma once

class metrics
{
public:
        metrics( microseconds time, float position, limits< float > position_range );

        void set_position_range( limits< float > position_range );

        void position_irq( microseconds now, float position );

        bool is_moving() const;

        float get_position() const;

        float get_velocity() const;

private:
        microseconds       last_time_;
        mtr::posvel_kalman pv_kal_;
        static_detector    st_dec_;
};
