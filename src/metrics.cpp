#include "metrics.hpp"

metrics::metrics( microseconds time, float position, limits< float > position_range )
  : last_time_( time )
  , pv_kal_( position )
{
        set_position_range( position_range );
}

float metrics::get_position() const
{
        return pv_kal_.get_position();
}
float metrics::get_velocity() const
{
        return pv_kal_.get_velocity();
}

void metrics::set_position_range( limits< float > position_range )
{
        pv_kal_.set_position_range( position_range );
}

void metrics::position_irq( microseconds now, float position )
{
        if ( now == last_time_ ) {
                return;
        }

        microseconds tdiff = now - last_time_;

        auto sdiff = std::chrono::duration_cast< sec_time >( tdiff );

        pv_kal_.update( sdiff, position );

        last_time_ = now;
}
