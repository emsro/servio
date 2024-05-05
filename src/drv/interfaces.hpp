#include "base/base.hpp"

#include <cstdint>
#include <emlabcpp/result.h>
#include <span>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

class detailed_cb_iface
{
public:
        virtual void on_value_irq( uint32_t val, std::span< uint16_t > profile ) = 0;
        virtual ~detailed_cb_iface()                                             = default;
};

class value_cb_iface
{
public:
        virtual void on_value_irq( uint32_t val ) = 0;
        virtual ~value_cb_iface()                 = default;
};

using current_cb_iface  = detailed_cb_iface;
using position_cb_iface = value_cb_iface;

class period_cb_iface
{
public:
        virtual void on_period_irq() = 0;
        virtual ~period_cb_iface()   = default;
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

struct com_iface
{
        virtual em::result start() = 0;
        virtual em::result
        send( std::span< const std::span< const std::byte > > data, microseconds timeout ) = 0;
        virtual com_res recv( std::span< std::byte > data )                                = 0;
        virtual ~com_iface() = default;
};

em::result send( com_iface& iface, microseconds timeout, auto&... data )
{
        std::array< std::span< const std::byte >, sizeof...( data ) > b{ data... };
        return iface.send( b, timeout );
}

struct leds_iface
{
        virtual void force_red_led()                 = 0;
        virtual void update( const leds_vals& leds ) = 0;
        virtual ~leds_iface()                        = default;
};

struct clk_iface
{
        virtual microseconds get_us() = 0;
        virtual ~clk_iface()          = default;
};

inline void wait_for( clk_iface& clk, microseconds ms )
{
        microseconds end = clk.get_us() + ms;
        while ( clk.get_us() < end )
                asm( "nop" );
}

class period_iface
{
public:
        virtual em::result       start()                                 = 0;
        virtual em::result       stop()                                  = 0;
        virtual void             set_period_callback( period_cb_iface& ) = 0;
        virtual period_cb_iface& get_period_callback()                   = 0;
        virtual ~period_iface()                                          = default;
};

class pwm_motor_iface
{
public:
        virtual void   force_stop()          = 0;
        virtual bool   is_stopped() const    = 0;
        virtual void   set_invert( bool v )  = 0;
        virtual void   set_power( pwr )      = 0;
        virtual int8_t get_direction() const = 0;
        virtual ~pwm_motor_iface()           = default;
};

class pos_iface
{
public:
        virtual uint32_t           get_position() const                        = 0;
        virtual void               set_position_callback( position_cb_iface& ) = 0;
        virtual position_cb_iface& get_position_callback() const               = 0;
        virtual ~pos_iface()                                                   = default;
};

class vcc_iface
{
public:
        virtual uint32_t get_vcc() const = 0;
        virtual ~vcc_iface()             = default;
};

class temp_iface
{
public:
        virtual uint32_t get_temperature() const = 0;
        virtual ~temp_iface()                    = default;
};

class curr_iface
{
public:
        virtual uint32_t          get_current() const                       = 0;
        virtual void              set_current_callback( current_cb_iface& ) = 0;
        virtual current_cb_iface& get_current_callback() const              = 0;
        virtual ~curr_iface()                                               = default;
};

}  // namespace servio::drv
