#include "metrics.hpp"

namespace servio::mtr
{

metrics::metrics( base::microseconds time, float position, base::limits< float > position_range )
  : last_time_( time )
  , pv_kal_( position, position_range )
  , st_dec_( position )
{
        set_position_range( position_range );
}

void metrics::set_position_range( base::limits< float > position_range )
{
        pv_kal_.set_position_range( position_range );
}

void metrics::set_moving_step( float step )
{
        st_dec_.set_step( step );
}

[[gnu::flatten]] void metrics::position_irq( base::microseconds now, float position )
{
        if ( now == last_time_ )
                return;

        const base::microseconds tdiff = now - last_time_;

        auto sdiff = std::chrono::duration_cast< base::sec_time >( tdiff );

        pv_kal_.update( sdiff, position );
        st_dec_.update( position );

        last_time_ = now;
}

}  // namespace servio::mtr
