#include <cstdint>
#include <span>

#pragma once

namespace fw::drv
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

}  // namespace fw::drv