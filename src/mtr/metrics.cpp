#include "metrics.hpp"

#include "../base.hpp"

#include <chrono>

namespace servio::mtr
{

metrics::metrics(
    microseconds    time,
    float           position,
    float           velocity,
    limits< float > position_range )
  : last_time_( time )
  , pv_kal_( position, velocity, position_range )
  , st_dec_( time, position )
{
        set_position_range( position_range );
}

void metrics::set_position_range( limits< float > position_range )
{
        pv_kal_.set_position_range( position_range );
        st_dec_.set_range( position_range.max() - position_range.min() );
}

void metrics::set_moving_step( float step )
{
        st_dec_.set_step( step );
}

void metrics::set_moving_window( float window )
{
        st_dec_.set_window( std::chrono::duration_cast< microseconds >( sec_time{ window } ) );
}

[[gnu::flatten]] void metrics::position_irq( microseconds now, float position )
{
        if ( now == last_time_ )
                return;

        microseconds const tdiff = now - last_time_;

        auto const sdiff = std::chrono::duration_cast< sec_time >( tdiff );

        pv_kal_.update( sdiff, position );
        st_dec_.update( now, position );

        last_time_ = now;
}

}  // namespace servio::mtr
