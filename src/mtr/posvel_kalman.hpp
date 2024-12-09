#include "../klmn/kalman.hpp"

#pragma once

namespace servio::mtr
{

struct posvel_kalman
{
        // TODO: hardcoded constant is meh
        static constexpr float observation_deviation = 0.000005F;
        static constexpr float process_deviation     = 0.005F;

        static constexpr klmn::observation_model            h = klmn::get_observation_model();
        static constexpr klmn::observation_noise_covariance r =
            klmn::get_observation_noise_covariance( observation_deviation );

        float             angle{};
        klmn::state_range st_range = { .offset = 0.F, .size = 0.F };

        klmn::state            x{};
        klmn::state_covariance P{};

        float offset = 0.F;

        posvel_kalman( float position, limits< float > position_range )
          : angle( position )
        {
                set_position_range( position_range );
                klmn::angle( x ) = klmn::angle_mod( position, st_range );
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
                return klmn::velocity( x );
        }

        void update( sec_time sdiff, float position )
        {
                // TODO: this is kinda non ideal, the sdiff should be stable in the system and
                // computed only once
                auto f = klmn::get_transition_model( sdiff );
                auto b = klmn::get_control_input_model( sdiff );
                auto q = klmn::get_process_noise_covariance( sdiff, process_deviation );

                klmn::control_input u{};
                u[0][0] = 0.F;
                klmn::observation z;
                klmn::angle( z ) = klmn::angle_mod( position + offset, st_range );

                std::tie( x, P ) = klmn::predict( x, P, u, f, b, q );
                std::tie( x, P ) = klmn::update( x, P, z, h, r );

                if ( klmn::requires_offset( klmn::angle( x ), st_range ) ) {
                        offset = klmn::angle_mod( offset + pi, st_range );
                        klmn::modify_angle( x, pi, st_range );
                }

                angle = klmn::angle_mod( klmn::angle( x ) - offset, st_range );
        }
};

}  // namespace servio::mtr
