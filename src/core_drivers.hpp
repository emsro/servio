#pragma once

#include "drv_interfaces.hpp"

#include <emlabcpp/experimental/function_view.h>

struct core_drivers
{
        clk_interface*                             clock;
        position_interface*                        position;
        current_interface*                         current;
        vcc_interface*                             vcc;
        temperature_interface*                     temperature;
        period_cb_interface*                       period_cb;  // TODO: maybe imrpove naming here?
        pwm_motor_interface*                       motor;
        period_interface*                          period;
        com_interface*                             comms;
        leds_interface*                            leds;
        em::function_view< void( core_drivers& ) > start_cb;

        auto tie()
        {
                return em::decompose( *this );
        }

        bool any_uninitialized()
        {
                return em::any_of( tie(), []< typename T >( const T ptr ) {
                        if constexpr ( std::is_pointer_v< T > ) {
                                return ptr == nullptr;
                        } else {
                                std::ignore = ptr;
                                return false;
                        }
                } );
        }
};
