#include "../iface/def.h"
#include "cfg/dispatcher.hpp"
#include "ctl/control.hpp"
#include "emlabcpp/outcome.h"
#include "fw/util.hpp"

#include <emlabcpp/experimental/function_view.h>

#pragma once

namespace servio::fw
{

struct dispatcher
{
        drv::pwm_motor_iface&                            motor;
        drv::pos_iface const&                            pos_drv;
        drv::curr_iface const&                           curr_drv;
        drv::vcc_iface const&                            vcc_drv;
        drv::temp_iface const&                           temp_drv;
        ctl::control&                                    ctl;
        mtr::metrics&                                    met;
        cfg::dispatcher&                                 cfg_disp;
        em::function_view< bool( cfg::map const* cfg ) > cfg_writer;
        cnv::converter&                                  conv;
        microseconds                                     now;
};

std::tuple< em::outcome, em::view< std::byte* > > handle_message(
    dispatcher&                  dis,
    em::view< std::byte const* > input_data,
    em::view< std::byte* >       output_buffer );

}  // namespace servio::fw
