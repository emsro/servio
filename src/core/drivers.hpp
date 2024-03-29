#pragma once

#include "drv/interfaces.hpp"

#include <emlabcpp/experimental/function_view.h>

namespace servio::core
{

namespace em = emlabcpp;

struct drivers
{
        drv::clk_iface*       clock;
        drv::pos_iface*       position;
        drv::curr_iface*      current;
        drv::vcc_iface*       vcc;
        drv::temp_iface*      temperature;
        drv::period_cb_iface* period_cb;  // TODO: maybe imrpove naming here?
        drv::pwm_motor_iface* motor;
        drv::period_iface*    period;
        drv::com_iface*       comms;
        drv::leds_iface*      leds;

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
