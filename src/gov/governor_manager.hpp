
#pragma once

#include "./governor.hpp"

namespace servio::gov
{

struct governor_manager
{
        static constexpr std::size_t n = 8;

        [[nodiscard]] bool add_gov( governor& gov )
        {
                for ( auto& g : governors_ ) {
                        if ( g == nullptr ) {
                                g = &gov;
                                return true;
                        }
                }
                return false;
        }

        bool activate( std::string_view name, std::span< std::byte > buffer )
        {
                status s = status::SUCCESS;
                for ( auto& g : governors_ ) {
                        if ( g && g->name() == name ) {
                                active_ = g;

                                std::tie( s, active_h_ ) = g->engage( buffer );
                                break;
                        }
                }
                return s == status::SUCCESS;
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

        governor* active_governor() const
        {
                return active_;
        }

private:
        std::array< governor*, n > governors_ = {};
        governor*                  active_    = nullptr;
        handle*                    active_h_  = nullptr;
};

}  // namespace servio::gov
