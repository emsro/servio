
#pragma once

#include "./governor.hpp"

#include <emlabcpp/pmr/stack_resource.hpp>
#include <emlabcpp/static_vector.hpp>
#include <iterator>

namespace servio::gov
{

struct governor_manager
{
        static constexpr std::size_t n = 8;

        [[nodiscard]] bool add_gov( governor& gov )
        {
                if ( governors_.full() )
                        return false;
                governors_.emplace_back( gov );
                return true;
        }

        status disengage()
        {
                if ( active_ && active_h_ ) {
                        auto const st = active_->disengage( *active_h_ );
                        active_       = nullptr;
                        active_h_     = nullptr;
                        return st;
                }
                return status::failure;
        }

        status activate(
            std::string_view          name,
            em::pmr::memory_resource& buffer,
            microseconds              now,
            float                     position,
            float                     velocity )
        {
                status s = status::error;
                if ( active_ ) {
                        if ( disengage() != status::success )
                                return status::error;
                }
                for ( vari::vref< governor > const g : governors_ ) {
                        if ( g->name() == name ) {
                                active_ = g.get();

                                std::tie( s, active_h_ ) =
                                    g->engage( buffer, now, position, velocity );
                                break;
                        }
                }
                return s;
        }

        status deactivate()
        {
                if ( active_ && active_h_ ) {
                        if ( active_->disengage( *active_h_ ) != status::success )
                                return status::error;
                        active_h_ = nullptr;
                        active_   = nullptr;
                        return status::success;
                }
                return status::failure;
        }

        [[gnu::flatten]] pwr current_irq( microseconds now, float current )
        {
                if ( active_h_ )
                        return active_h_->current_irq( now, current );
                return 0_pwr;
        }

        [[gnu::flatten]] void
        metrics_irq( microseconds now, float position, float velocity, bool is_moving )
        {
                if ( active_h_ )
                        active_h_->metrics_irq( now, position, velocity, is_moving );
        }

        governor* active() const
        {
                return active_;
        }

        std::span< vari::vref< governor > const > governors() const
        {
                return { governors_ };
        }

private:
        em::static_vector< vari::vref< governor >, n > governors_ = {};
        governor*                                      active_    = nullptr;
        handle*                                        active_h_  = nullptr;
};

inline status check_cfg_sections( zll::ll_list< cfg::iface >& cfgs )
{
        for ( auto i = cfgs.begin(); i != cfgs.end(); ++i ) {
                if ( i->section == cfg::root_cfg_section )
                        return status::error;
                for ( auto j = std::next( i ); j != cfgs.end(); ++j )
                        if ( j->section == i->section )
                                return status::error;
        }
        return status::success;
}

inline void create_governors(
    governor_manager&           gm,
    em::pmr::memory_resource&   mem,
    zll::ll_list< cfg::iface >& cfgs )
{
        auto f = [&]( governor_factory& fac ) {
                // XXX: error handling?
                auto* p = fac.create( mem );
                if ( !p )
                        return;
                if ( !gm.add_gov( *p ) )
                        return;
                if ( cfg::iface* c = p->get_cfg() ) {
                        c->section = p->cfg_section();
                        cfgs.link_back( *c );
                }
        };
        for_each_factory( f );
}

struct governors_ctx
{
        em::pmr::stack_resource< 1024 > mem;
        governor_manager                manager;
        zll::ll_list< cfg::iface >      cfgs;

        governors_ctx()
        {
                create_governors( manager, mem, cfgs );
                valid_ =
                    !manager.governors().empty() && check_cfg_sections( cfgs ) == status::success;
        }

        [[nodiscard]] bool valid() const
        {
                return valid_;
        }

private:
        bool valid_ = false;
};

}  // namespace servio::gov
