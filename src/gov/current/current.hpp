#pragma once

#include "../../cfg/handler.hpp"
#include "../governor.hpp"
#include "./cfg.hpp"
#include "./iface.hpp"

#include <emlabcpp/pid.h>

namespace servio::gov::curr
{

struct _current_gov final : governor, handle
{
        _current_gov()
          : pid( 0, { { .p = 1.F, .i = 0.F, .d = 0.F }, { -10.F, 10.F } } )
          , power( 0.F )
        {
        }

        std::string_view name() const override
        {
                return "current";
        }

        servio::cfg::iface* get_cfg() override
        {
                return &cfg_handler;
        };

        void apply_cfg( cfg::key k )
        {
                switch ( k ) {
                case cfg::key::curr_loop_p:
                case cfg::key::curr_loop_i:
                case cfg::key::curr_loop_d:
                        pid.cfg.coefficients = {
                            .p = cfg.curr_loop_p,
                            .i = cfg.curr_loop_i / pid_r,
                            .d = cfg.curr_loop_d * pid_r };
                        break;
                case cfg::key::curr_lim_min:
                case cfg::key::curr_lim_max:
                        em::update_limits( pid, { cfg.curr_lim_min, cfg.curr_lim_max } );
                        break;
                case cfg::key::pos_lim_min:
                case cfg::key::pos_lim_max:
                case cfg::key::pos_to_curr_lim_scale:
                        break;
                }
        }

        engage_res engage( em::pmr::memory_resource& ) override
        {
                power = pwr( 0.F );
                return { SUCCESS, this };
        }

        status disengage( handle& ) override
        {
                power = pwr( 0.F );
                return SUCCESS;
        }

        status on_cmd( iface::cmd_parser cmd, servio::iface::root_ser out ) override
        {
                auto [stm, st] = iface::parse_curr( std::move( cmd ) );
                if ( st != iface::parse_status::SUCCESS ) {
                        std::move( out ).nok()( "parse error" );
                        return ERROR;
                }

                using R = status;
                return stm.sub.visit( [&]( iface::set_stmt const& s ) -> R {
                        set_goal_current( s.goal );
                        std::move( out ).ok();
                        return SUCCESS;
                } );
        }

        void set_goal_current( float goal )
        {
                this->goal = goal;
        }

        pwr current_irq( microseconds now, float current ) override
        {
                auto  lims         = em::intersection( pid.cfg.limits, derived_curr_lims );
                float desired_curr = clamp( goal, lims );

                float const fpower = em::update( pid, now.count(), current, desired_curr );
                power              = pwr( fpower );
                return power;
        }

        void
        metrics_irq( microseconds, float position, float /*velocity*/, bool /*is_moving*/ ) override
        {
                derived_curr_lims.max() =
                    cfg.pos_to_curr_lim_scale * ( cfg.pos_lim_max - position );
                derived_curr_lims.min() =
                    cfg.pos_to_curr_lim_scale * ( cfg.pos_lim_min - position );
        }

        using pid_t   = em::pid< typename microseconds::rep >;
        using pid_per = microseconds::period;

        // magical constant to vary P and D coefficients of PID controller according to time units
        static constexpr auto pid_r = float{ pid_per::den } / float{ pid_per::num };

        cfg::map                         cfg;
        servio::cfg::handler< cfg::map > cfg_handler{
            cfg,
            em::member_function< &_current_gov::apply_cfg >{ *this } };

        float goal = 0.0F;
        pid_t pid;

        limits< float > derived_curr_lims;
        pwr             power;
};

}  // namespace servio::gov::curr
