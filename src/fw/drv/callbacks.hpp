#include "fw/drv/interfaces.hpp"

#include <cstdint>
#include <span>

#pragma once

namespace fw::drv
{

template < typename Callable >
struct adc_detailed_cb : public adc_detailed_cb_interface
{
        adc_detailed_cb( Callable cb )
          : cb( std::move( cb ) )
        {
        }

        void on_value_irq( uint32_t val, std::span< uint16_t > profile )
        {
                cb( val, profile );
        }

        Callable cb;
};

template < typename Callable >
using current_cb = adc_detailed_cb< Callable >;

struct empty_adc_detailed_cb : public adc_detailed_cb_interface
{
        void on_value_irq( uint32_t, std::span< uint16_t > )
        {
        }
};

using empty_current_cb = empty_adc_detailed_cb;

template < typename Callable >
struct value_cb : public value_cb_interface
{
        value_cb( Callable cb )
          : cb( std::move( cb ) )
        {
        }

        virtual void on_value_irq( uint32_t pos )
        {
                cb( pos );
        }

        Callable cb;
};
template < typename Callable >
using position_cb = value_cb< Callable >;

struct empty_value_cb : public value_cb_interface
{
        virtual void on_value_irq( uint32_t )
        {
        }
};

using empty_position_cb = empty_value_cb;

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
