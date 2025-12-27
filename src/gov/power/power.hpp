#pragma once

#include "../governor.hpp"
#include "./iface.hpp"

#include <emlabcpp/pid.h>

namespace servio::gov::pow
{

struct _power_gov final : governor, handle
{
        _power_gov() = default;

        std::string_view name() const override
        {
                return "power";
        }

        servio::cfg::iface* get_cfg() override
        {
                return nullptr;
        };

        engage_res engage( em::pmr::memory_resource& ) override
        {
                power = 0_pwr;
                return { SUCCESS, this };
        }

        status disengage( handle& ) override
        {
                power = 0_pwr;
                return SUCCESS;
        }

        status on_cmd( iface::cmd_parser cmd, servio::iface::root_ser out ) override
        {
                auto [stm, st] = iface::parse_pow( std::move( cmd ) );
                if ( st != iface::parse_status::SUCCESS ) {
                        std::move( out ).nok()( "parse error" );
                        return ERROR;
                }

                using R = status;
                return stm.sub.visit( [&]( iface::set_stmt const& s ) -> R {
                        power = pwr{ s.goal };
                        std::move( out ).ok();
                        return SUCCESS;
                } );
        }

        pwr current_irq( microseconds, float ) override
        {
                return power;
        }

        void metrics_irq(
            microseconds /*now*/,
            float /*position*/,
            float /*velocity*/,
            bool /*is_moving */ ) override
        {
        }

        using pid = em::pid< typename microseconds::rep >;

        pwr power = 0_pwr;
};

}  // namespace servio::gov::pow
