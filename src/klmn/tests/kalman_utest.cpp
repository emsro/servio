#include "klmn/kalman.hpp"
#include "klmn/simulate.hpp"

#include <gtest/gtest.h>
#include <random>

namespace servio::tests
{

TEST( Kalman, predict )
{

        const sec_time tdiff{ 1.F };
        const float    process_deviation = 0.005F;

        klmn::state                          x{};
        klmn::state_covariance               P{};
        const klmn::process_noise_covariance Q =
            klmn::get_process_noise_covariance( tdiff, process_deviation );
        const klmn::control_input_model    B = klmn::get_control_input_model( tdiff );
        const klmn::state_transition_model F = klmn::get_transition_model( tdiff );
        klmn::control_input                u;
        klmn::angle( u ) = 0.F;

        const std::size_t count    = 1000;
        const float       velocity = 0.1F;
        klmn::angle( x )           = 0.F;
        klmn::velocity( x )        = velocity;

        for ( const std::size_t i : em::range( count ) ) {
                std::ignore      = i;
                std::tie( x, P ) = klmn::predict( x, P, u, F, B, Q );
        }

        const float expected = velocity * static_cast< float >( count );
        EXPECT_NEAR( klmn::angle( x ), expected, 0.001F );
}

TEST( Kalman, base )
{

        const sec_time tdiff{ 0.1F };
        const float    process_deviation     = 0.0005F;
        const float    observation_deviation = 0.000005F;

        std::vector< klmn::observation > zs;
        klmn::observation                z{};
        const klmn::state_range          sr{ .offset = 0.F, .size = 2 * pi };
        float                            angle = 0.F;

        for ( const std::size_t i : em::range( 1000U ) ) {
                std::ignore = i;
                angle += 0.05F;
                angle            = klmn::angle_mod( angle, sr );
                klmn::angle( z ) = angle;
                zs.push_back( z );
        }
        std::vector< klmn::state > states =
            klmn::simulate( tdiff, process_deviation, observation_deviation, zs, sr );

        ASSERT_EQ( states.size(), zs.size() );

        for ( const std::size_t i : em::range( states.size() ) ) {
                if ( i < 100U ) {
                        continue;
                }
                EXPECT_NEAR( klmn::velocity( states[i] ), 0.5F, 0.01F );
        }
}

}  // namespace servio::tests
