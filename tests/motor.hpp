#include "emlabcpp/defer.h"

#include <array>
#include <iostream>

#pragma once

namespace servio::tests
{

using angle_vec = std::array< float, 2 >;

inline angle_vec rotate_vec( const angle_vec& v, float angle )
{
        return {
            std::cos( angle ) * v[0] - std::sin( angle ) * v[1],
            std::sin( angle ) * v[0] + std::cos( angle ) * v[1],
        };
}

struct simple_motor_sim
{
        int16_t   power    = 0;
        float     current  = 0.F;
        float     velocity = 0.F;
        angle_vec pos_vec  = { 1.F, 0.f };

        float last_t = 0.;

        float static_friction_vel   = 0.02f;
        float static_friction_force = 0.7f;

        simple_motor_sim() = default;

        simple_motor_sim( float pos )
        {
                pos_vec = rotate_vec( pos_vec, pos );
        }

        void reset_time( microseconds now )
        {
                last_t = static_cast< float >( now.count() ) / 1000'000.F;
        }

        void apply_power( microseconds now, int16_t p )
        {
                auto t = static_cast< float >( now.count() ) / 1000'000.F;

                power              = p;
                const float t_diff = t - last_t;

                if ( t_diff == 0.F ) {
                        return;
                }
                const em::defer d{ [&] {
                        last_t = t;
                } };

                current = power_to_current( p );

                const float force = current * 15.F;

                if ( std::abs( velocity ) < static_friction_vel ) {
                        if ( std::abs( force ) < static_friction_force ) {
                                velocity = 0.F;
                                return;
                        }
                }

                // let's simplify current into accel
                const float acc = force - resistance( velocity );

                velocity += acc * t_diff;

                const float angle_change = velocity * t_diff;
                pos_vec                  = rotate_vec( pos_vec, angle_change );
        }

        float position() const
        {
                return std::atan2( pos_vec[1], pos_vec[0] );
        }

        static float resistance( float vel )
        {
                return std::min( 0.05f, vel ) + vel * 0.1F;
        }

        static float power_to_current( int16_t p )
        {
                return em::map_range< int16_t, float >(
                    p,
                    std::numeric_limits< int16_t >::lowest(),
                    std::numeric_limits< int16_t >::max(),
                    -3.F,
                    3.F );
        }
};

inline std::ostream& operator<<( std::ostream& os, const simple_motor_sim& sim )
{
        return os << "\t" << sim.power << "\t" << sim.current << "\t" << sim.velocity << "\t"
                  << sim.position();
}

}  // namespace servio::tests
