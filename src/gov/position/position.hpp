#include "../../cfg/handler.hpp"
#include "../../lib/linear_transition_regulator.hpp"
#include "../current/current.hpp"
#include "../governor.hpp"
#include "./cfg.hpp"
#include "./iface.hpp"

#include <emlabcpp/pid.h>

namespace servio::gov::pos
{

struct _position_gov final : governor, handle
{
        _position_gov()
          : curr_pid( 0, { { .p = 1.F, .i = 0.F, .d = 0.F }, { -10.F, 10.F } } )
          , pos_pid( 0, { { .p = 1.F, .i = 0.F, .d = 0.F }, { -10.F, 10.F } } )
        {
        }

        std::string_view name() const override
        {
                return "position";
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
                        curr_pid.cfg.coefficients = {
                            .p = cfg.curr_loop_p,
                            .i = cfg.curr_loop_i / pid_r,
                            .d = cfg.curr_loop_d * pid_r };
                        break;
                case cfg::key::pos_loop_p:
                case cfg::key::pos_loop_i:
                case cfg::key::pos_loop_d:
                        pos_pid.cfg.coefficients = {
                            .p = cfg.pos_loop_p,
                            .i = cfg.pos_loop_i / pid_r,
                            .d = cfg.pos_loop_d * pid_r };
                        break;
                case cfg::key::static_friction_decay:
                case cfg::key::static_friction_scale:
                        current_scale_regl.set_config(
                            cfg.static_friction_scale, cfg.static_friction_decay );
                        break;
                case cfg::key::curr_lim_min:
                case cfg::key::curr_lim_max:
                        em::update_limits( curr_pid, { cfg.curr_lim_min, cfg.curr_lim_max } );
                        break;
                case cfg::key::pos_lim_min:
                case cfg::key::pos_lim_max:
                case cfg::key::pos_to_curr_lim_scale:
                        break;
                }
        }

        engage_res engage( em::pmr::memory_resource& ) override
        {
                goal_pos = 0.F;
                return { SUCCESS, this };
        }

        status disengage( handle& ) override
        {
                return SUCCESS;
        }

        status on_cmd( iface::cmd_parser cmd, servio::iface::root_ser out ) override
        {
                auto [stm, st] = iface::parse_pos( std::move( cmd ) );
                if ( st != iface::parse_status::SUCCESS ) {
                        std::move( out ).nok()( "parse error" );
                        return ERROR;
                }

                using R = status;
                return stm.sub.visit( [&]( iface::set_stmt const& s ) -> R {
                        goal_pos = s.goal;
                        std::move( out ).ok();
                        return SUCCESS;
                } );
        }

        pwr current_irq( microseconds now, float current ) override
        {
                auto  lims         = em::intersection( curr_pid.cfg.limits, derived_curr_lims );
                float desired_curr = clamp( goal_curr, lims );

                float const fpower = em::update( curr_pid, now.count(), current, desired_curr );
                auto        power_ = pwr( fpower );
                return power_;
        }

        void
        metrics_irq( microseconds now, float position, float /*velocity*/, bool is_moving ) override
        {
                derived_curr_lims.max() =
                    cfg.pos_to_curr_lim_scale * ( cfg.pos_lim_max - position );
                derived_curr_lims.min() =
                    cfg.pos_to_curr_lim_scale * ( cfg.pos_lim_min - position );

                current_scale_regl.update( now, is_moving );

                goal_curr = em::update( pos_pid, now.count(), position, goal_pos );
                goal_curr *= current_scale_regl.state;
        }

        using pid     = em::pid< typename microseconds::rep >;
        using pid_per = microseconds::period;

        // magical constant to vary P and D coefficients of PID controller according to time units
        static constexpr auto pid_r = float{ pid_per::den } / float{ pid_per::num };

        cfg::map                         cfg;
        servio::cfg::handler< cfg::map > cfg_handler{
            cfg,
            em::member_function< &_position_gov::apply_cfg >{ *this } };
        linear_transition_regulator current_scale_regl;

        limits< float > derived_curr_lims;

        float goal_curr = 0.0F;
        pid   curr_pid;
        float goal_pos = 0.0F;
        pid   pos_pid;
};

}  // namespace servio::gov::pos
