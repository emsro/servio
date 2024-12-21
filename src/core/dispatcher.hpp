#include "../cfg/dispatcher.hpp"
#include "../ctl/control.hpp"
#include "../iface/def.h"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/outcome.h>

#pragma once

namespace servio::core
{

struct dispatcher
{
        drv::motor_info_iface&     motor;
        drv::get_pos_iface const&  pos_drv;
        drv::get_curr_iface const& curr_drv;
        drv::vcc_iface const&      vcc_drv;
        drv::temp_iface const&     temp_drv;
        ctl::control&              ctl;
        mtr::metrics&              met;
        cfg::dispatcher&           cfg_disp;
        cfg::payload&              cfg_pl;
        drv::storage_iface&        stor_drv;
        cnv::converter&            conv;
        microseconds               now;
};

std::tuple< em::outcome, em::view< std::byte* > > handle_message(
    dispatcher&                  dis,
    em::view< std::byte const* > input_data,
    em::view< std::byte* >       output_buffer );

}  // namespace servio::core
