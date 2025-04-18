#include "../base.hpp"

#include <emlabcpp/experimental/matrix.h>

#pragma once

namespace servio::klmn
{

namespace em = emlabcpp;

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
        state_transition_model f;
        f[0] = { 1.F, tdiff.count() };
        f[1] = { 0, 1.F };
        return f;
}

constexpr control_input_model get_control_input_model( sec_time tdiff )
{
        control_input_model b;
        b[0] = { 0.F * tdiff.count() * tdiff.count() };
        b[1] = { tdiff.count() };
        return b;
}

constexpr process_noise_covariance
get_process_noise_covariance( sec_time tdiff, float standard_deviation )
{
        auto b = get_control_input_model( tdiff );

        return b * em::transpose( b ) * ( standard_deviation * standard_deviation );
}

constexpr observation_model get_observation_model()
{
        observation_model h;
        h[0] = { 1.F, 0.F };
        return h;
}

constexpr observation_noise_covariance
get_observation_noise_covariance( float observation_deviation )
{
        observation_noise_covariance r;
        r[0][0] = observation_deviation * observation_deviation;
        return r;
}

constexpr std::tuple< state, state_covariance > predict(
    state const&                    x_prev,
    state_covariance const&         p_prev,
    control_input const&            u,
    state_transition_model const&   f,
    control_input_model const&      b,
    process_noise_covariance const& q )
{
        state const            x = f * x_prev + b * u;
        state_covariance const p = f * p_prev * em::transpose( f ) + q;

        return { x, p };
}

constexpr std::tuple< state, state_covariance > update(
    state const&                        x_prev,
    state_covariance const&             p_prev,
    observation const&                  z,
    observation_model const&            h,
    observation_noise_covariance const& r )
{

        innovation_covariance const s = h * p_prev * em::transpose( h ) + r;
        kalman_gain const           k = p_prev * em::transpose( h ) * em::inverse( s );

        constexpr em::identity_matrix< 2 > i;

        state const            x = ( i - k * h ) * x_prev + k * z;
        state_covariance const p = ( i - k * h ) * p_prev;

        return { x, p };
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

struct state_range
{
        float offset;
        float size;
};

constexpr bool requires_offset( float const v, state_range const& r )
{
        return ( v < r.offset + 0.1F * r.size ) || ( r.offset + 0.8F * r.size < v );
}

inline float angle_mod( float v, state_range r )
{
        while ( v < 0.F )
                v += r.size;
        return std::fmod( v - r.offset, r.size ) + r.offset;
}

template < typename T >
inline void modify_angle( T& x, float val, state_range r )
{
        angle( x ) = angle_mod( angle( x ) + val, r );
}

}  // namespace servio::klmn
