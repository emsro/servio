#include "./interfaces.hpp"

#include <cstdint>
#include <span>

#pragma once

namespace servio::drv
{

template < typename Callable >
struct detailed_cb : public detailed_cb_iface
{
        detailed_cb( Callable callback )
          : cb( std::move( callback ) )
        {
        }

        void on_value_irq( uint32_t val, std::span< uint16_t > profile ) override
        {
                cb( val, profile );
        }

        Callable cb;
};

template < typename Callable >
using current_cb = detailed_cb< Callable >;

struct empty_detailed_cb : public detailed_cb_iface
{
        void on_value_irq( uint32_t, std::span< uint16_t > ) override
        {
        }
};

inline empty_detailed_cb EMPTY_DETAILED_CALLBACK;

using empty_current_cb = empty_detailed_cb;

template < typename Callable >
struct value_cb : public value_cb_iface
{
        value_cb( Callable callback )
          : cb( std::move( callback ) )
        {
        }

        void on_value_irq( uint32_t pos ) override
        {
                cb( pos );
        }

        Callable cb;
};

template < typename Callable >
using position_cb = value_cb< Callable >;

struct empty_value_cb : public value_cb_iface
{
        void on_value_irq( uint32_t ) override
        {
        }
};

inline empty_value_cb EMPTY_CALLBACK;

using empty_position_cb = empty_value_cb;

template < typename Callable >
struct period_cb : public period_cb_iface
{
        period_cb( Callable callback )
          : cb( std::move( callback ) )
        {
        }

        void on_period_irq() override
        {
                cb();
        }

        Callable cb;
};

struct empty_period_cb : public period_cb_iface
{
        void on_period_irq() override
        {
        }
};

}  // namespace servio::drv
