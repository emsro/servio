#include "kalman.hpp"

#include <gtest/gtest.h>
#include <random>

namespace servio::tests
{

TEST( Kalman, predict )
{

        const sec_time tdiff{ 1.F };
        const float    process_deviation = 0.005F;

        kalman::state                          x{};
        kalman::state_covariance               P{};
        const kalman::process_noise_covariance Q =
            kalman::get_process_noise_covariance( tdiff, process_deviation );
        const kalman::control_input_model    B = kalman::get_control_input_model( tdiff );
        const kalman::state_transition_model F = kalman::get_transition_model( tdiff );
        kalman::control_input                u;
        kalman::angle( u ) = 0.F;

        const std::size_t count    = 1000;
        const float       velocity = 0.1F;
        kalman::angle( x )         = 0.F;
        kalman::velocity( x )      = velocity;

        for ( const std::size_t i : em::range( count ) ) {
                std::ignore      = i;
                std::tie( x, P ) = kalman::predict( x, P, u, F, B, Q );
        }

        const float expected = velocity * static_cast< float >( count );
        EXPECT_NEAR( kalman::angle( x ), expected, 0.001F );
}

TEST( Kalman, base )
{

        const sec_time tdiff{ 0.1F };
        const float    process_deviation     = 0.0005F;
        const float    observation_deviation = 0.000005F;

        std::vector< kalman::observation > zs;
        kalman::observation                z{};
        const kalman::state_range          sr{ .offset = 0.F, .size = 2 * pi };
        float                              angle = 0.F;

        for ( const std::size_t i : em::range( 1000U ) ) {
                std::ignore = i;
                angle += 0.05F;
                angle              = kalman::angle_mod( angle, sr );
                kalman::angle( z ) = angle;
                zs.push_back( z );
        }
        std::vector< kalman::state > states =
            kalman::simulate( tdiff, process_deviation, observation_deviation, zs, sr );

        ASSERT_EQ( states.size(), zs.size() );

        for ( const std::size_t i : em::range( states.size() ) ) {
                if ( i < 100U ) {
                        continue;
                }
                EXPECT_NEAR( kalman::velocity( states[i] ), 0.5F, 0.01F );
        }
}

}  // namespace servio::tests
