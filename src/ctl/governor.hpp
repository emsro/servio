#pragma once

#include "../base.hpp"
#include "../status.hpp"

#include <span>
#include <string_view>

namespace servio::ctl
{

struct args_def
{
        std::span< std::string_view > args;
};

struct args_token
{
        std::span< std::string_view >                                args;
        std::span< std::pair< std::string_view, std::string_view > > kwargs;
};

struct args
{
        args( args_def& def, args_token& tok )
          : def_( def )
          , tok_( tok )
        {
        }

private:
        args_def&   def_;
        args_token& tok_;
};

struct governor
{
        virtual governor*& next()       = 0;
        virtual status     activate()   = 0;
        virtual status     deactivate() = 0;

        virtual status on_cmd( args_token ) = 0;

        virtual status on_cfg( std::string_view key, std::string_view val ) = 0;

        virtual void current_irq( microseconds now, float current )   = 0;
        virtual void velocity_irq( microseconds now, float velocity ) = 0;
        virtual void position_irq( microseconds now, float position ) = 0;

        virtual pwr get_power() const = 0;
};

struct governor_registry
{
        static governor_registry& instance()
        {
                static governor_registry instance;
                return instance;
        }

        void register_governor( governor& gov )
        {
                if ( !first_ ) {
                        first_ = &gov;
                        return;
                }
                governor* n = first_;
                while ( n->next() )
                        n = n->next();
                n->next() = &gov;
        }

private:
        governor* first_;

        governor_registry() = default;
};

struct governor_autoreg
{
        governor_autoreg( governor& gov )
        {
                governor_registry::instance().register_governor( gov );
        }
};

}  // namespace servio::ctl
