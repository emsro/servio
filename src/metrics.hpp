#include "base.hpp"
#include "mtr/posvel_kalman.hpp"

#include <array>
#include <tuple>

#pragma once

class metrics
{
public:
        metrics( microseconds time, float position, limits< float > position_range );

        void set_position_range( limits< float > position_range );

        void position_irq( microseconds now, float position );

        float get_position() const;

        float get_velocity() const;

private:
        microseconds       last_time_;
        mtr::posvel_kalman pv_kal_;
};
