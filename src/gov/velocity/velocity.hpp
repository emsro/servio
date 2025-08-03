#include "../../cfg/handler.hpp"
#include "../../lib/linear_transition_regulator.hpp"
#include "../current/current.hpp"
#include "../governor.hpp"
#include "./cfg.hpp"
#include "./iface.hpp"

#include <emlabcpp/pid.h>

namespace servio::gov::vel
{

struct _velocity_gov final : governor, handle
{
        _velocity_gov()
          : curr_pid_( 0, { { .p = 1.F, .i = 0.F, .d = 0.F }, { -10.F, 10.F } } )
          , vel_pid_( 0, { { .p = 1.F, .i = 0.F, .d = 0.F }, { -10.F, 10.F } } )
        {
        }

        std::string_view name() const override
        {
                return "velocity";
        }

        servio::cfg::iface& get_cfg() override
        {
                return cfg_handler_;
        };

        void apply_cfg( cfg::key k )
        {
                switch ( k ) {
                case cfg::key::curr_loop_p:
                case cfg::key::curr_loop_i:
                case cfg::key::curr_loop_d:
                        curr_pid_.cfg.coefficients = {
                            .p = cfg_.curr_loop_p, .i = cfg_.curr_loop_i, .d = cfg_.curr_loop_d };
                        break;
                case cfg::key::vel_loop_p:
                case cfg::key::vel_loop_i:
                case cfg::key::vel_loop_d:
                        vel_pid_.cfg.coefficients = {
                            .p = cfg_.vel_loop_p, .i = cfg_.vel_loop_i, .d = cfg_.vel_loop_d };
                        break;
                case cfg::key::static_friction_decay:
                case cfg::key::static_friction_scale:
                        current_scale_regl_.set_config(
                            cfg_.static_friction_scale, cfg_.static_friction_decay );
                        break;
                case cfg::key::curr_lim_min:
                case cfg::key::curr_lim_max:
                        em::update_limits( curr_pid_, { cfg_.curr_lim_min, cfg_.curr_lim_max } );
                        break;
                case cfg::key::vel_lim_min:
                case cfg::key::vel_lim_max:
                case cfg::key::vel_to_curr_lim_scale:
                        break;
                }
        }

        engage_res engage( std::span< std::byte > ) override
        {
                goal_vel_ = 0.F;
                return { SUCCESS, this };
        }

        status disengage( handle& ) override
        {
                goal_vel_ = 0.F;
                return SUCCESS;
        }

        status on_cmd( iface::cmd_parser cmd, servio::iface::root_ser out ) override
        {
                auto [stm, st] = iface::parse_vel( std::move( cmd ) );
                if ( st != iface::parse_status::SUCCESS ) {
                        std::move( out ).nok()( "parse error" );
                        return ERROR;
                }

                using R = status;
                return stm.sub.visit(
                    [&]( iface::set_stmt const& s ) -> R {
                            goal_vel_ = s.goal;
                            return SUCCESS;
                    },
                    [&]( iface::cfg_stmt const& s ) -> R {
                            s.sub.visit(
                                [&]( iface::cfg_set_stmt const& st ) {
                                        auto opt_k = servio::cfg::cmd_iface::on_cmd_set(
                                            cfg_, st.field, st.value.data, std::move( out ) );
                                        if ( opt_k )
                                                apply_cfg( *opt_k );
                                },
                                [&]( iface::cfg_get_stmt const& st ) {
                                        servio::cfg::cmd_iface::on_cmd_get(
                                            cfg_, st.field, std::move( out ) );
                                },
                                [&]( iface::cfg_list5_stmt const& st ) {
                                        servio::cfg::cmd_iface::on_cmd_list5< cfg::map >(
                                            st.offset, std::move( out ) );
                                } );
                            return SUCCESS;
                    } );
        }

        pwr current_irq( microseconds now, float current ) override
        {
                auto  lims         = em::intersection( curr_pid_.cfg.limits, derived_curr_lims_ );
                float desired_curr = clamp( goal_curr_, lims );

                float const fpower = em::update( curr_pid_, now.count(), current, desired_curr );
                auto        power_ = pwr( fpower );
                return power_;
        }

        void
        metrics_irq( microseconds now, float /*position*/, float velocity, bool is_moving ) override
        {
                derived_curr_lims_.max() =
                    cfg_.vel_to_curr_lim_scale * ( cfg_.vel_lim_max - velocity );
                derived_curr_lims_.min() =
                    cfg_.vel_to_curr_lim_scale * ( cfg_.vel_lim_min - velocity );

                current_scale_regl_.update( now, is_moving );

                goal_curr_ = em::update( vel_pid_, now.count(), velocity, goal_vel_ );
                goal_curr_ *= current_scale_regl_.state;
        }

private:
        using pid = em::pid< typename microseconds::rep >;

        cfg::map                         cfg_;
        servio::cfg::handler< cfg::map > cfg_handler_{ cfg_ };
        linear_transition_regulator      current_scale_regl_;

        limits< float > derived_curr_lims_;

        float goal_vel_ = 0.0F;

        float goal_curr_ = 0.0F;
        pid   curr_pid_;
        pid   vel_pid_;
};

}  // namespace servio::gov::vel
