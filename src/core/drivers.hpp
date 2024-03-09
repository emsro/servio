#pragma once

#include "drv/interfaces.hpp"

#include <emlabcpp/experimental/function_view.h>

namespace servio::core
{

namespace em = emlabcpp;

struct drivers
{
        drv::clk_interface*                         clock;
        drv::position_interface*                    position;
        drv::current_interface*                     current;
        drv::vcc_interface*                         vcc;
        drv::temperature_interface*                 temperature;
        drv::period_cb_interface*                   period_cb;  // TODO: maybe imrpove naming here?
        drv::pwm_motor_interface*                   motor;
        drv::period_interface*                      period;
        drv::com_interface*                         comms;
        drv::leds_interface*                        leds;
        em::function_view< em::result( drivers& ) > start_cb;

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

}  // namespace servio::core
