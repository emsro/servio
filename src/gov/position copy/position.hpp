#include "../../cfg/handler.hpp"
#include "../../lib/linear_transition_regulator.hpp"
#include "../current/current.hpp"
#include "../governor.hpp"
#include "./cfg.hpp"
#include "./iface.hpp"

#include <emlabcpp/pid.h>

namespace servio::gov::pos
{

std::tuple< iface::stmt, iface::parse_status > parse_curr( iface::cmd_parser p );

struct _position_gov final : governor, handle
{
        _position_gov()
          : goal_pos_( 0.F )
          , pid_( 0, { { .p = 1.F, .i = 0.F, .d = 0.F }, { -10.F, 10.F } } )
        {
        }

        std::string_view name() const override
        {
                return "position";
        }

        servio::cfg::iface& get_cfg() override
        {
                return cfg_handler_;
        };

        void apply_cfg( cfg::key k )
        {
                switch ( k ) {
                case cfg::key::loop_p:
                case cfg::key::loop_i:
                case cfg::key::loop_d:
                        pid_.cfg.coefficients = {
                            .p = cfg_.loop_p, .i = cfg_.loop_i, .d = cfg_.loop_d };
                        break;
                case cfg::key::static_friction_decay:
                case cfg::key::static_friction_scale:
                        current_scale_regl_.set_config(
                            cfg_.static_friction_scale, cfg_.static_friction_decay );
                        break;
                }
        }

        engage_res engage( std::span< std::byte > ) override
        {
                curr::current_gvnr.set_goal_current( 0.F );
                return { SUCCESS, this };
        }

        status disengage( handle& ) override
        {
                curr::current_gvnr.set_goal_current( 0.F );
                return SUCCESS;
        }

        status on_cmd( iface::cmd_parser cmd, servio::iface::root_ser out ) override
        {
                auto [stm, st] = parse_curr( std::move( cmd ) );
                // XXX: do we want to reply? :)
                if ( st != iface::parse_status::SUCCESS )
                        return ERROR;

                using R = status;
                return stm.sub.visit(
                    [&]( iface::set_stmt const& s ) -> R {
                            goal_ = s.goal;
                            return SUCCESS;
                    },
                    [&]( iface::cfg_stmt const& s ) -> R {
                            s.sub.visit(
                                [&]( iface::cfg_set_stmt const& st ) {
                                        auto opt_k = servio::cfg::on_cmd_set(
                                            cfg_, st.field, st.value.data, std::move( out ) );
                                        if ( opt_k )
                                                apply_cfg( *opt_k );
                                },
                                [&]( iface::cfg_get_stmt const& st ) {
                                        servio::cfg::on_cmd_get( cfg_, st.field, std::move( out ) );
                                },
                                [&]( iface::cfg_list5_stmt const& st ) {
                                        servio::cfg::on_cmd_list5< cfg::map >(
                                            st.offset, std::move( out ) );
                                } );
                            return SUCCESS;
                    } );
        }

        pwr current_irq( microseconds now, float position ) override
        {
                return curr::current_gvnr.current_irq( now, position );
        }

        void metrics_irq( microseconds now, float position, float /*velocity*/, bool /*is_moving*/ )
            override
        {
                float curr = em::update( pid_, now.count(), position, goal_ );
                curr *= current_scale_regl_.state;
                curr::current_gvnr.set_goal_current( curr );
        }

private:
        using pid = em::pid< typename microseconds::rep >;

        cfg::map                         cfg_;
        servio::cfg::handler< cfg::map > cfg_handler_{ cfg_ };
        linear_transition_regulator      current_scale_regl_;

        float goal_pos_;
        pid   pos_pid_;
};

extern _position_gov position_gnvr;

}  // namespace servio::gov::pos
