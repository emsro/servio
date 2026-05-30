#include "../kalman.hpp"
#include "../simulate.hpp"

#include <cmath>
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

namespace
{

constexpr float       seam_quantum = 2.3841858e-7F;
constexpr sec_time    seam_tdiff{ 0.001F };
constexpr float       seam_process_deviation     = 0.5F;
constexpr float       seam_observation_deviation = 0.005F;
constexpr std::size_t seam_sample_count          = 800U;
constexpr std::size_t seam_warmup_samples        = 200U;

float quantize_angle( float angle )
{
        return std::round( angle / seam_quantum ) * seam_quantum;
}

std::vector< observation > make_positive_motion_observations( float initial_angle )
{
        constexpr float input_velocity = 0.01F;

        std::vector< observation > zs;
        zs.reserve( seam_sample_count );

        state_range const sr{ .offset = 0.F, .size = 2 * pi };
        float             angle = initial_angle;
        for ( std::size_t i = 0; i < seam_sample_count; ++i ) {
                std::ignore = i;
                angle       = angle_mod( angle + input_velocity * seam_tdiff.count(), sr );
                observation z{};
                klmn::angle( z ) = quantize_angle( angle );
                zs.push_back( z );
        }

        return zs;
}

std::vector< observation > make_stationary_dither_observations( float initial_angle )
{
        std::vector< observation > zs;
        zs.reserve( seam_sample_count );

        state_range const sr{ .offset = 0.F, .size = 2 * pi };
        for ( std::size_t i = 0; i < seam_sample_count; ++i ) {
                float jitter = 0.F;
                switch ( i % 4U ) {
                case 0U:
                        jitter = -2.F * seam_quantum;
                        break;
                case 1U:
                        jitter = 0.F;
                        break;
                case 2U:
                        jitter = 2.F * seam_quantum;
                        break;
                default:
                        jitter = 0.F;
                        break;
                }

                observation z{};
                klmn::angle( z ) = quantize_angle( angle_mod( initial_angle + jitter, sr ) );
                zs.push_back( z );
        }

        return zs;
}

std::vector< state > simulate_states( std::vector< observation > const& zs )
{
        state_range const sr{ .offset = 0.F, .size = 2 * pi };
        return simulate( seam_tdiff, seam_process_deviation, seam_observation_deviation, zs, sr );
}

float average_velocity_from_states( std::vector< state > const& states )
{
        float       sum     = 0.F;
        std::size_t samples = 0U;
        for ( std::size_t i = 0; i < states.size(); ++i ) {
                if ( i < seam_warmup_samples )
                        continue;
                sum += velocity( states[i] );
                samples += 1U;
        }

        return sum / static_cast< float >( samples );
}

}  // namespace

/// Verifies that the Kalman position/velocity estimator reports the same
/// positive velocity for the same forward motion sequence in mid-range and
/// when the observations start close to the 0/2*pi overlap seam.
TEST( Kalman, velocity_near_overlap_matches_midrange )
{
        float const mid_avg = average_velocity_from_states(
            simulate_states( make_positive_motion_observations( 3.0F ) ) );
        float const seam_avg = average_velocity_from_states(
            simulate_states( make_positive_motion_observations( 2 * pi - 0.0005F ) ) );

        EXPECT_GT( mid_avg, 0.F );
        EXPECT_GT( seam_avg, 0.F );
        EXPECT_NEAR( seam_avg, mid_avg, 0.001F );
}

/// Verifies that seam-adjacent quantized observations do not create a spurious
/// velocity bias compared to the same stationary dither sequence in mid-range.
TEST( Kalman, stationary_dither_near_overlap_matches_midrange )
{
        float const mid_avg = average_velocity_from_states(
            simulate_states( make_stationary_dither_observations( 3.0F ) ) );
        float const seam_avg = average_velocity_from_states(
            simulate_states( make_stationary_dither_observations( 2 * pi - seam_quantum ) ) );

        EXPECT_NEAR( mid_avg, 0.F, 0.00005F );
        EXPECT_NEAR( seam_avg, 0.F, 0.00005F );
        EXPECT_NEAR( seam_avg, mid_avg, 0.00005F );
}

}  // namespace servio::klmn::tests
