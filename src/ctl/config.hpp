#include <emlabcpp/pid.h>

#ifdef EMLABCPP_USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

// TODO: is this even used?

#pragma once

namespace ctl
{

using pid              = em::pid< typename microseconds::rep >;
using pid_conf         = typename pid::config;
using pid_coefficients = em::pid_coefficients;

struct config
{
        ctl::pid_coefficients position_pid;
        ctl::pid_coefficients velocity_pid;
        ctl::pid_coefficients current_pid;

        limits< float > position_limits;
        limits< float > velocity_limits;
        limits< float > current_limits;

        float static_friction_scale;
        float static_friction_decay;
};

}  // namespace ctl

#ifdef EMLABCPP_USE_NLOHMANN_JSON
template <>
struct nlohmann::adl_serializer< ctl::config >
{
        static void to_json( nlohmann::json& j, const ctl::config& cfg )
        {
                j["position_pid"]          = cfg.position_pid;
                j["velocity_pid"]          = cfg.velocity_pid;
                j["current_pid"]           = cfg.current_pid;
                j["position_limits"]       = cfg.position_limits;
                j["velocity_limits"]       = cfg.velocity_limits;
                j["current_limits"]        = cfg.current_limits;
                j["static_friction_scale"] = cfg.static_friction_scale;
                j["static_friction_decay"] = cfg.static_friction_decay;
        }

        static ctl::config from_json( const nlohmann::json& j )
        {
                ctl::config cfg;
                cfg.position_pid          = j["position_pid"];
                cfg.velocity_pid          = j["velocity_pid"];
                cfg.current_pid           = j["current_pid"];
                cfg.position_limits       = j["position_limits"];
                cfg.velocity_limits       = j["velocity_limits"];
                cfg.current_limits        = j["current_limits"];
                cfg.static_friction_scale = j["static_friction_scale"];
                cfg.static_friction_decay = j["static_friction_decay"];
                return cfg;
        }
};

#endif
