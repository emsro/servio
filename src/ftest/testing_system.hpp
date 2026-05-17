#include "../base.hpp"
#include "../drv/interfaces.hpp"
#include "./utest.hpp"

#pragma once

namespace servio::ftest
{

struct testing_system
{
        drv::com_iface& debug_comms;

        asrt_reac_assm assm;

        std::byte buff[1024];

        testing_system( drv::com_iface& debug_comms, char const* name )
          : debug_comms( debug_comms )
        {
                auto r = asrt::init( assm, name, 1000 );
                if ( r != ASRT_SUCCESS )
                        fw::stop_exec();
        }

        void tick( microseconds now )
        {
                milliseconds ms = duration_cast< milliseconds >( now );
                asrt::tick( assm, (uint32_t) ms.count() );

                while ( auto req = asrt::next( assm.send_queue ) ) {
                        asrt_rec_span* b = &req->buff;

                        auto f = [&b] -> std::span< std::byte const > {
                                if ( !b )
                                        return {};
                                std::span< std::byte const > sp{
                                    (std::byte*) b->b, (std::byte*) b->e };
                                b = b->next;
                                return sp;
                        };
                        auto st = debug_comms.send( f, 100_ms );

                        req.finish( st == status::success ? ASRT_SUCCESS : ASRT_SEND_ERR );
                }

                auto [succ, data] = debug_comms.recv( buff );
                if ( !succ )
                        return;

                auto r = asrt_chann_dispatch(
                    &assm.reactor.node,
                    asrt::span{
                        .b = (uint8_t*) data.begin(),
                        .e = (uint8_t*) data.begin() + data.size() } );
                if ( r != ASRT_SUCCESS )
                        fw::stop_exec();
        }
};

}  // namespace servio::ftest
