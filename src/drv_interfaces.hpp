#include "status_category.hpp"

#include <cstdint>
#include <span>

#pragma once

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

enum class status
{
        NOMINAL,
        DATA_ACQUISITION_ERROR,
        DATA_ACQUISITION_CRITICAL_ERROR,
};

constexpr status_category status_category_of( status status )
{
        switch ( status ) {
        case status::NOMINAL:
                return status_category::NOMINAL;
        case status::DATA_ACQUISITION_ERROR:
                return status_category::DEGRADED;
        case status::DATA_ACQUISITION_CRITICAL_ERROR:
                return status_category::INOPERABLE;
        }
};

class driver_interface
{
public:
        virtual status get_status() const = 0;
        virtual void   clear_status( status ){};
        virtual ~driver_interface() = default;
};

class period_interface : public driver_interface
{
public:
        // TODO: maybe the void wor start/end is not a great idea?
        virtual void                 start()                                     = 0;
        virtual void                 stop()                                      = 0;
        virtual void                 set_period_callback( period_cb_interface& ) = 0;
        virtual period_cb_interface& get_period_callback()                       = 0;
};

class pwm_motor_interface : public driver_interface
{
public:
        virtual void   set_power( int16_t )  = 0;
        virtual int8_t get_direction() const = 0;
};

class position_interface : public driver_interface
{
public:
        virtual uint32_t               get_position() const                            = 0;
        virtual void                   set_position_callback( position_cb_interface& ) = 0;
        virtual position_cb_interface& get_position_callback() const                   = 0;
};

class vcc_interface : public driver_interface

{
public:
        virtual uint32_t get_vcc() const = 0;
};

class temperature_interface : public driver_interface

{
public:
        virtual uint32_t get_temperature() const = 0;
};

class current_interface : public driver_interface

{
public:
        virtual uint32_t              get_current() const                           = 0;
        virtual void                  set_current_callback( current_cb_interface& ) = 0;
        virtual current_cb_interface& get_current_callback() const                  = 0;
};
