#include "klmn/kalman.hpp"
#include "klmn/simulate.hpp"

#include <gtest/gtest.h>
#include <random>

namespace servio::klmn::tests
{

TEST( Kalman, predict )
{

        const base::sec_time tdiff{ 1.F };
        const float          process_deviation = 0.005F;

        state                          x{};
        state_covariance               P{};
        const process_noise_covariance Q = get_process_noise_covariance( tdiff, process_deviation );
        const control_input_model      B = get_control_input_model( tdiff );
        const state_transition_model   F = get_transition_model( tdiff );
        control_input                  u;
        angle( u ) = 0.F;

        const std::size_t count = 1000;
        const float       vel   = 0.1F;
        angle( x )              = 0.F;
        velocity( x )           = vel;

        for ( const std::size_t i : em::range( count ) ) {
                std::ignore      = i;
                std::tie( x, P ) = predict( x, P, u, F, B, Q );
        }

        const float expected = vel * static_cast< float >( count );
        EXPECT_NEAR( angle( x ), expected, 0.001F );
}

TEST( Kalman, base )
{

        const base::sec_time tdiff{ 0.1F };
        const float          process_deviation     = 0.0005F;
        const float          observation_deviation = 0.000005F;

        std::vector< observation > zs;
        observation                z{};
        const state_range          sr{ .offset = 0.F, .size = 2 * base::pi };
        float                      alpha = 0.F;

        for ( const std::size_t i : em::range( 1000U ) ) {
                std::ignore = i;
                alpha += 0.05F;
                alpha      = angle_mod( alpha, sr );
                angle( z ) = alpha;
                zs.push_back( z );
        }
        std::vector< state > states =
            simulate( tdiff, process_deviation, observation_deviation, zs, sr );

        ASSERT_EQ( states.size(), zs.size() );

        for ( const std::size_t i : em::range( states.size() ) ) {
                if ( i < 100U ) {
                        continue;
                }
                EXPECT_NEAR( velocity( states[i] ), 0.5F, 0.01F );
        }
}

}  // namespace servio::klmn::tests
