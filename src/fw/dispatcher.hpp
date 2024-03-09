#include "cfg/dispatcher.hpp"
#include "ctl/control.hpp"
#include "emlabcpp/outcome.h"
#include "fw/util.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <io.pb.h>

#pragma once

namespace servio::fw
{

struct dispatcher
{
        drv::pwm_motor_interface&                        motor;
        const drv::position_interface&                   pos_drv;
        const drv::current_interface&                    curr_drv;
        const drv::vcc_interface&                        vcc_drv;
        const drv::temperature_interface&                temp_drv;
        ctl::control&                                    ctl;
        mtr::metrics&                                    met;
        cfg::dispatcher&                                 cfg_disp;
        em::function_view< bool( const cfg::map* cfg ) > cfg_writer;
        cnv::converter&                                  conv;
        base::microseconds                               now;
};

ServioToHost handle_message( dispatcher& dis, const HostToServio& msg );

std::tuple< em::outcome, em::view< std::byte* > > handle_message(
    dispatcher&                  dis,
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer );

std::tuple< em::outcome, em::view< std::byte* > > handle_message_packet(
    dispatcher&                  dis,
    em::view< const std::byte* > input_data,
    em::view< std::byte* >       output_buffer );

}  // namespace servio::fw
