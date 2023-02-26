#include <array>
#include <iostream>

#pragma once

using angle_vec = std::array< float, 2 >;

angle_vec rotate_vec( const angle_vec& v, float angle )
{
        return {
            std::cos( angle ) * v[0] - std::sin( angle ) * v[1],
            std::sin( angle ) * v[0] + std::cos( angle ) * v[1],
        };
}

struct simple_motor_sim
{
        int16_t   power    = 0;
        float     current  = 0.f;
        float     velocity = 0.f;
        angle_vec pos_vec  = { 1.f, 0.f };

        float last_t = 0.;

        void reset_time( std::chrono::microseconds now )
        {
                last_t = static_cast< float >( now.count() ) / 1000'000.f;
        }

        void apply_power( std::chrono::microseconds now, int16_t p )
        {
                auto t = static_cast< float >( now.count() ) / 1000'000.f;

                power        = p;
                float t_diff = t - last_t;

                if ( t_diff == 0.f ) {
                        return;
                }

                current = power_to_current( p );

                // let's simplify current into accel
                float acc = current * 2.5f - resistance( current );

                velocity += acc * t_diff;

                float angle_change = velocity * t_diff;
                pos_vec            = rotate_vec( pos_vec, angle_change );

                last_t = t;
        }

        float position() const
        {
                return std::atan2( pos_vec[1], pos_vec[0] );
        }

        float resistance( float current ) const
        {
                std::ignore = current;
                return velocity * 0.01f;
        }

        float power_to_current( int16_t p ) const
        {
                return em::map_range< int16_t, float >(
                    p,
                    std::numeric_limits< int16_t >::lowest(),
                    std::numeric_limits< int16_t >::max(),
                    -3.f,
                    3.f );
        }
};

std::ostream& operator<<( std::ostream& os, const simple_motor_sim& sim )
{
        return os << "\t" << sim.power << "\t" << sim.current << "\t" << sim.velocity << "\t"
                  << sim.position();
}
