
#include "./base.h"

#include <emlabcpp/experimental/string_buffer.h>
#include <vari/vopt.h>
#include <vari/vval.h>

#pragma once

namespace servio::iface
{
namespace em = emlabcpp;

using mode = vari::typelist<
    field< 1, "disengaged"_a, void >,
    field< 2, "power"_a, float >,
    field< 3, "current"_a, float >,
    field< 4, "velocity"_a, float >,
    field< 5, "position"_a, float > >;

using mode_types = typename field_traits< mode >::types;
using mode_vals  = typename field_traits< mode >::vals;
using mode_key   = typename field_traits< mode >::keys;

struct mode_stmt
{
        vari::vval< mode_vals > arg;

        constexpr auto operator<=>( mode_stmt const& ) const noexcept = default;
};

using property = vari::typelist<
    field< 1, "mode"_a, mode_key >,
    field< 2, "current"_a, float >,
    field< 3, "vcc"_a, float >,
    field< 4, "temp"_a, float >,
    field< 5, "position"_a, float >,
    field< 6, "velocity"_a, float > >;

using prop_traits = field_traits< property >;
using prop_types  = prop_traits::types;
using prop_vals   = prop_traits::vals;
using prop_key    = prop_traits::keys;

struct prop_stmt
{
        prop_key prop;

        constexpr auto operator<=>( prop_stmt const& ) const noexcept = default;
};

using encoder_mode = vari::typelist<  //
    field< 1, "analog"_a, void >,
    field< 2, "quad"_a, void > >;

using ec_mode_key = typename field_traits< encoder_mode >::keys;

using cfg = vari::typelist<
    field< 1, "model"_a, std::string_view >,
    field< 2, "id"_a, uint32_t >,
    field< 3, "group_id"_a, uint32_t >,
    field< 4, "encoder_mode"_a, ec_mode_key >,
    field< 11, "position_lower_angle"_a, float >,
    field< 12, "position_higher_angle"_a, float >,
    field< 14, "current_conv_scale"_a, float >,
    field< 15, "current_conv_offset"_a, float >,
    field< 16, "temp_conv_scale"_a, float >,
    field< 17, "temp_conv_offset"_a, float >,
    field< 18, "voltage_conv_scale"_a, float >,
    field< 19, "invert_hbridge"_a, bool >,
    field< 30, "current_loop_p"_a, float >,
    field< 31, "current_loop_i"_a, float >,
    field< 32, "current_loop_d"_a, float >,
    field< 33, "current_lim_min"_a, float >,
    field< 34, "current_lim_max"_a, float >,
    field< 40, "velocity_loop_p"_a, float >,
    field< 41, "velocity_loop_i"_a, float >,
    field< 42, "velocity_loop_d"_a, float >,
    field< 43, "velocity_lim_min"_a, float >,
    field< 44, "velocity_lim_max"_a, float >,
    field< 45, "velocity_to_current_lim_scale"_a, float >,
    field< 50, "position_loop_p"_a, float >,
    field< 51, "position_loop_i"_a, float >,
    field< 52, "position_loop_d"_a, float >,
    field< 53, "position_lim_min"_a, float >,
    field< 54, "position_lim_max"_a, float >,
    field< 55, "position_to_velocity_lim_scale"_a, float >,
    field< 60, "static_friction_scale"_a, float >,
    field< 61, "static_friction_decay"_a, float >,
    field< 62, "moving_detection_step"_a, float >,
    field< 65, "minimum_voltage"_a, float >,
    field< 66, "maximum_temperature"_a, float >,
    field< 80, "quad_encoder_range"_a, uint32_t > >;

using cfg_traits = field_traits< cfg >;
using cfg_types  = typename cfg_traits::types;
using cfg_vals   = typename cfg_traits::vals;
using cfg_key    = typename cfg_traits::keys;

struct cfg_set_stmt
{
        vari::vval< cfg_vals > val;

        constexpr auto operator<=>( cfg_set_stmt const& ) const noexcept = default;
};

struct cfg_get_stmt
{
        cfg_key k;

        constexpr auto operator<=>( cfg_get_stmt const& ) const noexcept = default;
};

struct cfg_commit_stmt
{
        constexpr auto operator<=>( cfg_commit_stmt const& ) const noexcept = default;
};

struct cfg_clear_stmt
{
        constexpr auto operator<=>( cfg_clear_stmt const& ) const noexcept = default;
};

struct invalid_stmt
{
        constexpr auto operator<=>( invalid_stmt const& ) const noexcept = default;
};

using stmt = vari::
    typelist< mode_stmt, prop_stmt, cfg_set_stmt, cfg_get_stmt, cfg_commit_stmt, cfg_clear_stmt >;

// XXX: missing ID targeting for servos

struct resp
{
        em::string_buffer< 64 > res;
};

vari::vval< stmt, invalid_stmt > parse( std::string_view inpt );

}  // namespace servio::iface
