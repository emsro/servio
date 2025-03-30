#include "../base.hpp"
#include "../cfg/map.hpp"
#include "../status.hpp"

#include <cstdint>
#include <span>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

struct detailed_cb_iface
{
        virtual void on_value_irq( uint32_t val, std::span< uint16_t > profile ) = 0;
};

struct value_cb_iface
{
        virtual void on_value_irq( uint32_t val ) = 0;
};

using current_cb_iface  = detailed_cb_iface;
using position_cb_iface = value_cb_iface;

struct period_cb_iface
{
        virtual void on_period_irq() = 0;
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

using send_data_t = std::span< std::span< std::byte const > const >;

struct com_iface
{
        virtual status  start()                                        = 0;
        virtual status  send( send_data_t data, microseconds timeout ) = 0;
        virtual com_res recv( std::span< std::byte > data )            = 0;
};

status send( com_iface& iface, microseconds timeout, auto&... data )
{
        std::array< std::span< std::byte const >, sizeof...( data ) > b{ data... };
        return iface.send( b, timeout );
}

struct storage_iface
{
        virtual status store_page( std::span< std::byte const > data ) = 0;
        virtual status load_page( std::span< std::byte > data )        = 0;
};

struct leds_iface
{
        virtual void force_red_led()                 = 0;
        virtual void update( leds_vals const& leds ) = 0;
};

struct clk_iface
{
        virtual microseconds get_us() = 0;
};

inline void wait_for( clk_iface& clk, microseconds ms )
{
        microseconds end = clk.get_us() + ms;
        while ( clk.get_us() < end )
                asm( "nop" );
}

inline bool spin_with_timeout( clk_iface& clk, bool volatile& cond, microseconds timeout )
{
        auto end = clk.get_us() + timeout;
        while ( !cond )
                if ( clk.get_us() > end )
                        return false;
        return true;
}

struct period_iface
{
        virtual status           start()                                 = 0;
        virtual status           stop()                                  = 0;
        virtual void             set_period_callback( period_cb_iface& ) = 0;
        virtual period_cb_iface& get_period_callback()                   = 0;
};

struct motor_info_iface
{
        virtual bool   is_stopped() const    = 0;
        virtual int8_t get_direction() const = 0;
};

struct pwm_motor_iface : motor_info_iface
{
        virtual void force_stop()         = 0;
        virtual void set_invert( bool v ) = 0;
        virtual void set_power( pwr )     = 0;
};

struct get_pos_iface
{
        virtual limits< uint32_t > get_position_range() const = 0;
        virtual uint32_t           get_position() const       = 0;
};

struct pos_iface : get_pos_iface
{
        virtual void               set_position_callback( position_cb_iface& ) = 0;
        virtual position_cb_iface& get_position_callback() const               = 0;
};

struct vcc_iface
{
        virtual uint32_t get_vcc() const = 0;
};

struct temp_iface
{
        virtual int32_t get_temperature() const = 0;
        virtual void    tick(){};
};

struct get_curr_iface
{
        virtual uint32_t get_current() const = 0;
};

struct curr_iface : get_curr_iface
{
        virtual void              set_current_callback( current_cb_iface& ) = 0;
        virtual current_cb_iface& get_current_callback() const              = 0;
};

}  // namespace servio::drv
