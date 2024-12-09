#include "./effects.hpp"

#include <emlabcpp/algorithm.h>

namespace em = emlabcpp;

namespace servio::mon
{

bool blinker::update( microseconds now )
{
        if ( now < next_e )
                return output;

        std::size_t state_i = i / 2;

        output = i % 2 == 0 && state[state_i];
        state.reset( state_i );

        i = ( i + 1 ) % ( n * 2 );

        next_e += step;

        return output;
}

float sin_approx( float x )
{
        float const x3 = x * x * x;
        return x - x3 / 6.F + ( x3 * x * x ) / 120.F - ( x3 * x3 * x ) / 5040.F;
}

uint8_t pulser::update()
{
        if ( val > pipi )
                val -= pipi;
        float v = ( sin_approx( val - pi ) + 1.F ) / 2.F;
        v *= intensity;

        return static_cast< uint8_t >( em::map_range( v, 0.F, 1.F, 0.F, 255.F ) );
}

}  // namespace servio::mon
