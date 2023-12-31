#pragma once

#include "base/drv_interfaces.hpp"

#include <emlabcpp/experimental/function_view.h>

namespace servio
{

namespace em = emlabcpp;

struct core_drivers
{
        base::clk_interface*         clock;
        base::position_interface*    position;
        base::current_interface*     current;
        base::vcc_interface*         vcc;
        base::temperature_interface* temperature;
        base::period_cb_interface*   period_cb;  // TODO: maybe imrpove naming here?
        base::pwm_motor_interface*   motor;
        base::period_interface*      period;
        base::com_interface*         comms;
        base::leds_interface*        leds;
        em::function_view< em::result( core_drivers& ) > start_cb;

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

}  // namespace servio
