
#pragma once

#include "./governor.hpp"

#include <emlabcpp/static_vector.h>

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
                        auto st   = active_->disengage( *active_h_ );
                        active_   = nullptr;
                        active_h_ = nullptr;
                        return st;
                }
                return status::FAILURE;
        }

        status activate( std::string_view name, em::pmr::memory_resource& buffer )
        {
                status s = status::ERROR;
                if ( active_ ) {
                        if ( disengage() != SUCCESS )
                                return status::ERROR;
                }
                for ( vari::vref< governor > g : governors_ ) {
                        if ( g->name() == name ) {
                                active_ = g.get();

                                std::tie( s, active_h_ ) = g->engage( buffer );
                                break;
                        }
                }
                return s;
        }

        status deactivate()
        {
                if ( active_ && active_h_ ) {
                        if ( active_->disengage( *active_h_ ) != SUCCESS )
                                return status::ERROR;
                        active_h_ = nullptr;
                        active_   = nullptr;
                        return status::SUCCESS;
                }
                return status::FAILURE;
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

inline void create_governors( governor_manager& gm, em::pmr::memory_resource& mem )
{
        auto f = [&]( governor_factory& fac ) {
                // XXX: error handling?
                auto* p = fac.create( mem );
                if ( !p )
                        return;
                if ( !gm.add_gov( *p ) )
                        return;
        };
        for_each_factory( f );
}

}  // namespace servio::gov
