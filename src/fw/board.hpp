#include "config.hpp"
#include "fw/drv/clock.hpp"
#include "fw/drv/cobs_uart.hpp"
#include "fw/drv/hbridge.hpp"
#include "fw/drv/leds.hpp"
#include "globals.hpp"

#pragma once

// TODO: eeeehe, this si not exactly right folder for this namespace
namespace brd
{

/// TODO: maybe move to references and make setup function return an optional?
struct core_drivers
{
        fw::drv::clock*                 clock;
        fw::drv::position_interface*    position;
        fw::drv::current_interface*     current;
        fw::drv::vcc_interface*         vcc;
        fw::drv::temperature_interface* temperature;
        fw::drv::period_cb_interface*   period_cb;  // TODO: maybe imrpove naming here?
        fw::drv::hbridge*               hbridge;
        fw::drv::cobs_uart*             comms;
        fw::drv::leds*                  leds;
        void ( *start_cb )( core_drivers& );

        auto tie()
        {
                return em::decompose( *this );
        }

        bool any_uninitialized()
        {
                return em::any_of( tie(), []( const auto ptr ) {
                        return ptr == nullptr;
                } );
        }
};

em::view< const em::view< std::byte* >* > get_persistent_pages();
cfg_map                                   get_config();

void setup_board();

fw::drv::cobs_uart* setup_debug_comms();
core_drivers        setup_core_drivers();

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
