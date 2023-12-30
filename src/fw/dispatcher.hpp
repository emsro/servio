#include "cfg/dispatcher.hpp"
#include "control.hpp"
#include "fw/util.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <io.pb.h>

#pragma once

namespace servio::fw
{

struct dispatcher
{
        pwm_motor_interface&                             motor;
        const position_interface&                        pos_drv;
        const current_interface&                         curr_drv;
        const vcc_interface&                             vcc_drv;
        const temperature_interface&                     temp_drv;
        control&                                         ctl;
        metrics&                                         met;
        cfg::dispatcher&                                 cfg_disp;
        em::function_view< bool( const cfg::map* cfg ) > cfg_writer;
        converter&                                       conv;
        microseconds                                     now;
};

ServioToHost handle_message( dispatcher& dis, const HostToServio& msg );

std::tuple< bool, em::view< std::byte* > > handle_message(
    dispatcher&                  dis,
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer );

std::tuple< bool, em::view< std::byte* > > handle_message_packet(
    dispatcher&                  dis,
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer );

}  // namespace servio::fw
