#include "kalman.hpp"

#pragma once

namespace servio::mtr
{

struct posvel_kalman
{
        // TODO: hardcoded constant is meh
        static constexpr float observation_deviation = 0.000005F;
        static constexpr float process_deviation     = 0.005F;

        static constexpr kalman::observation_model            h = kalman::get_observation_model();
        static constexpr kalman::observation_noise_covariance r =
            kalman::get_observation_noise_covariance( observation_deviation );

        float               angle{};
        kalman::state_range st_range = { .offset = 0.F, .size = 0.F };

        kalman::state            x{};
        kalman::state_covariance P{};

        float offset = 0.F;

        posvel_kalman( float position, limits< float > position_range )
          : angle( position )
        {
                set_position_range( position_range );
                kalman::angle( x ) = kalman::angle_mod( position, st_range );
        }

        void set_position_range( limits< float > position_range )
        {
                st_range.offset = position_range.min();
                st_range.size   = position_range.max() - position_range.min();
        }

        float get_position() const
        {
                return angle;
        }

        float get_velocity() const
        {
                return kalman::velocity( x );
        }

        void update( sec_time sdiff, float position )
        {
                // TODO: this is kinda non ideal, the sdiff should be stable in the system and
                // computed only once
                auto f = kalman::get_transition_model( sdiff );
                auto b = kalman::get_control_input_model( sdiff );
                auto q = kalman::get_process_noise_covariance( sdiff, process_deviation );

                kalman::control_input u{};
                u[0][0] = 0.F;
                kalman::observation z;
                kalman::angle( z ) = kalman::angle_mod( position + offset, st_range );

                std::tie( x, P ) = kalman::predict( x, P, u, f, b, q );
                std::tie( x, P ) = kalman::update( x, P, z, h, r );

                if ( kalman::requires_offset( kalman::angle( x ), st_range ) ) {
                        offset = kalman::angle_mod( offset + pi, st_range );
                        kalman::modify_angle( x, pi, st_range );
                }

                angle = kalman::angle_mod( kalman::angle( x ) - offset, st_range );
        }
};

}  // namespace servio::mtr
