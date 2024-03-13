#include "base/base.hpp"

#include <cstdint>
#include <emlabcpp/result.h>
#include <span>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

class adc_detailed_cb_interface
{
public:
        virtual void on_value_irq( uint32_t val, std::span< uint16_t > profile ) = 0;
        virtual ~adc_detailed_cb_interface()                                     = default;
};

class value_cb_interface
{
public:
        virtual void on_value_irq( uint32_t val ) = 0;
        virtual ~value_cb_interface()             = default;
};

using current_cb_interface  = adc_detailed_cb_interface;
using position_cb_interface = value_cb_interface;

class period_cb_interface
{
public:
        virtual void on_period_irq()   = 0;
        virtual ~period_cb_interface() = default;
};

struct com_res
{
        bool                   success;
        em::view< std::byte* > used_data;

        operator std::tuple< bool&, em::view< std::byte* >& >()
        {
                return std::tie( success, used_data );
        }
};

class com_interface
{
public:
        virtual em::result start() = 0;
        virtual em::result
                        send( em::view< const std::byte* > data, base::microseconds timeout ) = 0;
        virtual com_res load_message( em::view< std::byte* > data )                           = 0;
        virtual ~com_interface() = default;
};

class leds_interface
{
public:
        virtual void force_red_led()                       = 0;
        virtual void update( const base::leds_vals& leds ) = 0;
        virtual ~leds_interface()                          = default;
};

class clk_interface
{
public:
        virtual base::microseconds get_us() = 0;
        virtual ~clk_interface()            = default;
};

inline void wait_for( clk_interface& clk, base::microseconds ms )
{
        base::microseconds end = clk.get_us() + ms;
        while ( end < clk.get_us() )
                asm( "nop" );
}

class period_interface
{
public:
        virtual em::result           start()                                     = 0;
        virtual em::result           stop()                                      = 0;
        virtual void                 set_period_callback( period_cb_interface& ) = 0;
        virtual period_cb_interface& get_period_callback()                       = 0;
        virtual ~period_interface()                                              = default;
};

class pwm_motor_interface
{
public:
        virtual void   force_stop()          = 0;
        virtual bool   is_stopped() const    = 0;
        virtual void   set_power( int16_t )  = 0;
        virtual int8_t get_direction() const = 0;
        virtual ~pwm_motor_interface()       = default;
};

class position_interface
{
public:
        virtual uint32_t               get_position() const                            = 0;
        virtual void                   set_position_callback( position_cb_interface& ) = 0;
        virtual position_cb_interface& get_position_callback() const                   = 0;
        virtual ~position_interface()                                                  = default;
};

class vcc_interface
{
public:
        virtual uint32_t get_vcc() const = 0;
        virtual ~vcc_interface()         = default;
};

class temperature_interface
{
public:
        virtual uint32_t get_temperature() const = 0;
        virtual ~temperature_interface()         = default;
};

class current_interface
{
public:
        virtual uint32_t              get_current() const                           = 0;
        virtual void                  set_current_callback( current_cb_interface& ) = 0;
        virtual current_cb_interface& get_current_callback() const                  = 0;
        virtual ~current_interface()                                                = default;
};

}  // namespace servio::drv
