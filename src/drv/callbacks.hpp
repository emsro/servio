#include "base/drv_interfaces.hpp"

#include <cstdint>
#include <span>

#pragma once

namespace servio::drv
{

template < typename Callable >
struct adc_detailed_cb : public base::adc_detailed_cb_interface
{
        adc_detailed_cb( Callable callback )
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
using current_cb = adc_detailed_cb< Callable >;

struct empty_adc_detailed_cb : public base::adc_detailed_cb_interface
{
        void on_value_irq( uint32_t, std::span< uint16_t > ) override
        {
        }
};

using empty_current_cb = empty_adc_detailed_cb;

template < typename Callable >
struct value_cb : public base::value_cb_interface
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

struct empty_value_cb : public base::value_cb_interface
{
        void on_value_irq( uint32_t ) override
        {
        }
};

using empty_position_cb = empty_value_cb;

template < typename Callable >
struct period_cb : public base::period_cb_interface
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

struct empty_period_cb : public base::period_cb_interface
{
        void on_period_irq() override
        {
        }
};

}  // namespace servio::drv
