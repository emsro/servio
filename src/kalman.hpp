#include "base.hpp"
#include "emlabcpp/experimental/matrix.h"

#pragma once

namespace em = emlabcpp;

namespace kalman
{

using state                        = em::matrix< 2, 1 >;
using state_transition_model       = em::matrix< 2, 2 >;
using state_covariance             = em::matrix< 2, 2 >;
using process_noise_covariance     = em::matrix< 2, 2 >;
using control_input_model          = em::matrix< 2, 1 >;
using control_input                = em::matrix< 1, 1 >;
using innovation_covariance        = em::matrix< 1, 1 >;
using kalman_gain                  = em::matrix< 2, 1 >;
using observation                  = em::matrix< 1, 1 >;
using observation_model            = em::matrix< 1, 2 >;
using observation_noise_covariance = em::matrix< 1, 1 >;

constexpr state_transition_model get_transition_model( sec_time tdiff )
{
    state_transition_model F;
    F[0] = { 1.f, tdiff.count() };
    F[1] = { 0, 1.f };
    return F;
}

constexpr control_input_model get_control_input_model( sec_time tdiff )
{
    control_input_model B;
    B[0] = { 0.f * tdiff.count() * tdiff.count() };
    B[1] = { tdiff.count() };
    return B;
}

constexpr process_noise_covariance
get_process_noise_covariance( sec_time tdiff, float standard_deviation )
{
    auto B = get_control_input_model( tdiff );

    return B * em::transpose( B ) * ( standard_deviation * standard_deviation );
}

constexpr observation_model get_observation_model()
{
    observation_model H;
    H[0] = { 1.f, 0.f };
    return H;
}

constexpr observation_noise_covariance
get_observation_noise_covariance( float observation_deviation )
{
    observation_noise_covariance R;
    R[0][0] = observation_deviation * observation_deviation;
    return R;
}

constexpr std::tuple< state, state_covariance > predict(
    const state&                    x_prev,
    const state_covariance&         P_prev,
    const control_input&            u,
    const state_transition_model&   F,
    const control_input_model&      B,
    const process_noise_covariance& Q )
{
    state            x = F * x_prev + B * u;
    state_covariance P = F * P_prev * em::transpose( F ) + Q;

    return { x, P };
}

constexpr std::tuple< state, state_covariance > update(
    const state&                        x_prev,
    const state_covariance&             P_prev,
    const observation&                  z,
    const observation_model&            H,
    const observation_noise_covariance& R )
{

    innovation_covariance S = H * P_prev * em::transpose( H ) + R;
    kalman_gain           K = P_prev * em::transpose( H ) * em::inverse( S );

    em::identity_matrix< 2 > I;

    state            x = ( I - K * H ) * x_prev + K * z;
    state_covariance P = ( I - K * H ) * P_prev;

    return { x, P };
}

template < typename T >
inline auto& angle( T& x )
{
    return x[0][0];
}
template < typename T >
inline auto& velocity( T& x )
{
    return x[1][0];
}

constexpr float PI = std::numbers::pi_v< float >;

inline bool requires_offset( const float v )
{
    return ( v < 0.4f * PI ) || ( 1.6f * PI < v );
}

inline float angle_mod( float v )
{
    if ( v < 0.f ) {
        return angle_mod( v + 2.f * PI );
    }
    return std::fmod( v, 2.f * PI );
}

template < typename T >
inline void modify_angle( T& x, float val )
{
    angle( x ) = angle_mod( angle( x ) + val );
}

inline std::vector< state > simulate(
    sec_time                          tdiff,
    float                             process_deviation,
    float                             observation_deviation,
    const std::vector< observation >& observations )
{
    state_transition_model       F = get_transition_model( tdiff );
    control_input_model          B = get_control_input_model( tdiff );
    process_noise_covariance     Q = get_process_noise_covariance( tdiff, process_deviation );
    observation_model            H = get_observation_model();
    observation_noise_covariance R = get_observation_noise_covariance( observation_deviation );

    state x;
    angle( x )    = angle( observations[0] );
    velocity( x ) = 0.f;
    state_covariance P;

    control_input u;
    angle( u ) = 0.f;

    std::vector< state > res;

    float offset = 0.f;
    for ( observation z : observations ) {
        modify_angle( z, +offset );

        std::tie( x, P ) = kalman::predict( x, P, u, F, B, Q );
        std::tie( x, P ) = kalman::update( x, P, z, H, R );

        if ( requires_offset( angle( x ) ) ) {
            offset = angle_mod( offset + PI );
            modify_angle( x, +PI );
        }

        res.push_back( x );
        modify_angle( res.back(), -offset );
    }

    return res;
}

}  // namespace kalman
