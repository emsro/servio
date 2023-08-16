#include "config.hpp"
#include "drv_interfaces.hpp"
#include "fw/drv/leds.hpp"
#include "globals.hpp"

#pragma once

// TODO: eeeehe, this si not exactly right folder for this namespace
namespace brd
{

/// TODO: maybe move to references and make setup function return an optional?
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
        fw::drv::leds*                             leds;
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

em::view< const em::view< std::byte* >* > get_persistent_pages();
cfg_map                                   get_config();

void setup_board();

com_interface* setup_debug_comms();
core_drivers   setup_core_drivers();

}  // namespace brd

namespace fw
{
inline void install_stop_callback( drv::leds* leds_ptr )
{
        if ( leds_ptr != nullptr ) {
                STOP_CALLBACK = [leds_ptr] {
                        leds_ptr->force_red_led();
                };
        }
};
}  // namespace fw
