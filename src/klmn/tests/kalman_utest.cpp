#include "../kalman.hpp"
#include "../simulate.hpp"

#include <gtest/gtest.h>
#include <random>

namespace servio::klmn::tests
{

TEST( Kalman, predict )
{

        sec_time const tdiff{ 1.F };
        float const    process_deviation = 0.005F;

        state                          x{};
        state_covariance               P{};
        process_noise_covariance const Q = get_process_noise_covariance( tdiff, process_deviation );
        control_input_model const      B = get_control_input_model( tdiff );
        state_transition_model const   F = get_transition_model( tdiff );
        control_input                  u;
        angle( u ) = 0.F;

        std::size_t const count = 1000;
        float const       vel   = 0.1F;
        angle( x )              = 0.F;
        velocity( x )           = vel;

        for ( std::size_t const i : em::range( count ) ) {
                std::ignore      = i;
                std::tie( x, P ) = predict( x, P, u, F, B, Q );
        }

        float const expected = vel * static_cast< float >( count );
        EXPECT_NEAR( angle( x ), expected, 0.001F );
}

TEST( Kalman, base )
{

        sec_time const tdiff{ 0.1F };
        float const    process_deviation     = 0.0005F;
        float const    observation_deviation = 0.000005F;

        std::vector< observation > zs;
        observation                z{};
        state_range const          sr{ .offset = 0.F, .size = 2 * pi };
        float                      alpha = 0.F;

        for ( std::size_t const i : em::range( 1000U ) ) {
                std::ignore = i;
                alpha += 0.05F;
                alpha      = angle_mod( alpha, sr );
                angle( z ) = alpha;
                zs.push_back( z );
        }
        std::vector< state > states =
            simulate( tdiff, process_deviation, observation_deviation, zs, sr );

        ASSERT_EQ( states.size(), zs.size() );

        for ( std::size_t const i : em::range( states.size() ) ) {
                if ( i < 100U )
                        continue;
                EXPECT_NEAR( velocity( states[i] ), 0.5F, 0.01F );
        }
}

}  // namespace servio::klmn::tests
