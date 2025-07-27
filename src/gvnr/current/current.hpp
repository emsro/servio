#include "../../cfg/handler.hpp"
#include "../governor.hpp"
#include "./cfg.hpp"

#include <emlabcpp/pid.h>

namespace servio::gvnr::curr
{

struct _current_gov : governor, handle
{
        _current_gov()
          : goal_( 0.F )
          , pid_( 0, { { .p = 1.F, .i = 0.F, .d = 0.F }, { -10.F, 10.F } } )
          , power_( 0.F )
        {
        }

        std::string_view name() const override
        {
                return "current";
        }

        servio::cfg::iface& get_cfg() override
        {
                return _cfg_handler;
        };

        status apply_cfg( cfg::key k )
        {
                switch ( k ) {
                case cfg::key::loop_p:
                case cfg::key::loop_i:
                case cfg::key::loop_d:
                        pid_.cfg.coefficients = {
                            .p = _cfg.loop_p, .i = _cfg.loop_i, .d = _cfg.loop_d };
                        break;
                case cfg::key::curr_lim_min:
                case cfg::key::curr_lim_max:
                        em::update_limits( pid_, { _cfg.curr_lim_min, _cfg.curr_lim_max } );
                        break;
                case cfg::key::pos_lim_min:
                case cfg::key::pos_lim_max:
                case cfg::key::pos_to_curr_lim_scale:
                        break;
                }
                return SUCCESS;
        }

        engage_res engage( std::span< std::byte > ) override
        {
                power_ = pwr( 0.F );
                return { SUCCESS, this };
        }

        status disengage( handle& ) override
        {
                power_ = pwr( 0.F );
                return SUCCESS;
        }

        status on_cmd( iface::cmd_parser cmd ) override
        {
        }

        void set_goal_current( float goal )
        {
                goal_ = goal;
        }

        void current_irq( microseconds now, float current ) override
        {
                auto  lims         = em::intersection( pid_.cfg.limits, derived_curr_lims_ );
                float desired_curr = clamp( goal_, lims );

                float const fpower = em::update( pid_, now.count(), current, desired_curr );
                power_             = pwr( fpower );
        }

        void
        metrics_irq( microseconds, float position, float /*velocity*/, bool /*is_moving*/ ) override
        {
                derived_curr_lims_.max() =
                    _cfg.pos_to_curr_lim_scale * ( _cfg.pos_lim_max - position );
                derived_curr_lims_.min() =
                    _cfg.pos_to_curr_lim_scale * ( _cfg.pos_lim_min - position );
        }

        pwr get_power() const override
        {
                return power_;
        }

private:
        using pid = em::pid< typename microseconds::rep >;

        cfg::map                         _cfg;
        servio::cfg::handler< cfg::map > _cfg_handler{ _cfg };

        limits< float > derived_curr_lims_;
        float           goal_;
        pid             pid_;
        pwr             power_;
};

extern _current_gov current_gnvr;

}  // namespace servio::gvnr::curr
