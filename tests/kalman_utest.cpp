#include "kalman.hpp"

#include <gtest/gtest.h>
#include <random>

TEST( Kalman, predict )
{

        std::random_device rd{};
        std::mt19937       gen{ rd() };

        sec_time tdiff{ 1.f };
        float    process_deviation = 0.005f;

        std::normal_distribution< float > od{ 0, process_deviation };

        kalman::state                    x{};
        kalman::state_covariance         P{};
        kalman::process_noise_covariance Q =
            kalman::get_process_noise_covariance( tdiff, process_deviation );
        kalman::control_input_model    B = kalman::get_control_input_model( tdiff );
        kalman::state_transition_model F = kalman::get_transition_model( tdiff );
        kalman::control_input          u;
        kalman::angle( u ) = 0.f;

        std::size_t count     = 1000;
        float       velocity  = 0.1f;
        kalman::angle( x )    = 0.f;
        kalman::velocity( x ) = velocity;

        for ( std::size_t i = 0; i < count; i++ ) {
                std::tie( x, P ) = kalman::predict( x, P, u, F, B, Q );
        }

        float expected = velocity * static_cast< float >( count );
        EXPECT_NEAR( kalman::angle( x ), expected, 0.001f );
}

TEST( Kalman, base )
{

        std::random_device rd{};
        std::mt19937       gen{ rd() };

        sec_time tdiff{ 0.1f };
        float    process_deviation     = 0.0005f;
        float    observation_deviation = 0.000005f;

        std::normal_distribution< float > od{ 0, observation_deviation };

        std::vector< kalman::observation > zs;
        kalman::observation                z{};
        kalman::state_range                sr{ .offset = 0.f, .size = 2 * pi };
        float                              angle = 0.f;

        for ( std::size_t i = 0; i < 1000; i++ ) {
                angle += 0.05f;
                angle              = kalman::angle_mod( angle, sr );
                kalman::angle( z ) = angle;
                zs.push_back( z );
        }
        std::vector< kalman::state > states =
            kalman::simulate( tdiff, process_deviation, observation_deviation, zs, sr );

        ASSERT_EQ( states.size(), zs.size() );

        for ( std::size_t i : em::range( states.size() ) ) {
                if ( i < 100u ) {
                        continue;
                }
                EXPECT_NEAR( kalman::velocity( states[i] ), 0.5, 0.01 );
        }
}