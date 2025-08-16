#pragma once

#include "../../core/core.hpp"
#include "../../core/dispatcher.hpp"
#include "../port.hpp"
#include "./util.hpp"

#include <deque>

namespace servio::scmdio
{

struct port_mock : port_iface
{
        struct attrs
        {
                drv::pwm_motor_iface&      motor;
                drv::get_pos_iface&        pos_drv;
                drv::get_curr_iface const& curr_drv;
                drv::vcc_iface const&      vcc_drv;
                drv::temp_iface const&     temp_drv;
                core::core&                cor;
                cfg::map&                  cfg_map;
                drv::storage_iface&        stor_drv;
        };

        port_mock( attrs const& attr )
          : a( attr )
        {
        }

        attrs a;

        std::deque< std::vector< std::byte > > buff;

        boost::asio::awaitable< void > write_msg( std::span< std::byte const > msg ) override
        {
                core::dispatcher disp{
                    .motor    = a.motor,
                    .pos_drv  = a.pos_drv,
                    .curr_drv = a.curr_drv,
                    .vcc_drv  = a.vcc_drv,
                    .temp_drv = a.temp_drv,
                    .gov      = a.cor.gov_,
                    .met      = a.cor.met,
                    .mon      = a.cor.mon,
                    .cfg_map  = a.cfg_map,
                    .stor_drv = a.stor_drv,
                    .conv     = a.cor.conv,
                    .now      = 0_ms,  // XXX: rethink
                };
                std::byte tmp[666];
                auto [res, used] = core::handle_message( disp, msg, tmp );
                assert( res == SUCCESS );
                buff.emplace_back( used.begin(), used.end() );
                co_return;
        }

        boost::asio::awaitable< std::span< std::byte > >
        read_msg( std::span< std::byte > buffer ) override
        {
                if ( buff.empty() )
                        co_return std::span< std::byte >{};

                assert( buff.front().size() <= buffer.size() );
                auto [in, out] = std::ranges::copy( buff.front(), buffer.begin() );
                buff.pop_front();
                co_return std::span< std::byte >{ buffer.begin(), out };
        }
};

}  // namespace servio::scmdio
