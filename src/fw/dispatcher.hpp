#include "control.hpp"
#include "fw/cfg_dispatcher.hpp"
#include "fw/conversion.hpp"
#include "fw/drv/hbridge.hpp"

#include <io.pb.h>

#pragma once

namespace fw
{

struct dispatcher
{
        drv::hbridge&                                   hb;
        const drv::position_interface&                  pos_drv;
        const drv::current_interface&                   curr_drv;
        const drv::vcc_interface&                       vcc_drv;
        const drv::temperature_interface&               temp_drv;
        control&                                        ctl;
        metrics&                                        met;
        cfg_dispatcher&                                 cfg_disp;
        em::function_view< bool( const cfg_map* cfg ) > cfg_writer;
        converter&                                      conv;
        microseconds                                    now;
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

}  // namespace fw
