#include "fw/drv/interfaces.hpp"

#include <cstdint>
#include <span>

#pragma once

namespace fw::drv
{

template < typename Callable >
struct current_cb : public current_cb_interface
{
        current_cb( Callable cb )
          : cb( std::move( cb ) )
        {
        }

        void on_current_irq( uint32_t current, std::span< uint16_t > profile )
        {
                cb( current, profile );
        }

        Callable cb;
};

struct empty_current_cb : public current_cb_interface
{
        void on_current_irq( uint32_t, std::span< uint16_t > )
        {
        }
};

template < typename Callable >
struct position_cb : public position_cb_interface
{
        position_cb( Callable cb )
          : cb( std::move( cb ) )
        {
        }

        virtual void on_position_irq( uint32_t pos )
        {
                cb( pos );
        }

        Callable cb;
};

struct empty_position_cb : public position_cb_interface
{
        virtual void on_position_irq( uint32_t )
        {
        }
};

template < typename Callable >
struct period_cb : public period_cb_interface
{
        period_cb( Callable cb )
          : cb( std::move( cb ) )
        {
        }

        virtual void on_period_irq()
        {
                cb();
        }

        Callable cb;
};

struct empty_period_cb : public period_cb_interface
{
        virtual void on_period_irq()
        {
        }
};

}  // namespace fw::drv
