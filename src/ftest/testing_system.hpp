#include "../base.hpp"
#include "../drv/interfaces.hpp"
#include "../ftester/base.hpp"
#include "./utest.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/parameters.h>
#include <emlabcpp/experimental/testing/reactor.h>

#pragma once

namespace servio::ftest
{

struct testing_system
{
        drv::com_iface& debug_comms;

        em::testing::reactor    reactor;
        em::testing::collector  collector;
        em::testing::parameters parameters;

        em::static_function< em::result( std::span< std::byte const > ), 32 > ctx_cb;

        uctx ctx;

        auto send_cb()
        {
                return [this]( uint16_t channel, auto const& data ) {
                        return send_( channel, data );
                };
        }

        testing_system( drv::com_iface& debug_comms, std::string_view name )
          : debug_comms( debug_comms )
          , reactor( em::testing::core_channel, name, send_cb() )
          , collector( em::testing::collect_channel, send_cb() )
          , parameters( em::testing::params_channel, send_cb() )
          , ctx_cb( [&]( std::span< std::byte const > data ) {
                  return send_( ftester::rec_id, data );
          } )
          , ctx( collector, ctx_cb )
        {
        }

        em::result send_( uint16_t channel, auto const& data )
        {
                auto hdr = em::protocol::multiplexer_channel_handler::serialize( channel );
                return send( debug_comms, 100_ms, hdr, data );
        }

        void tick()
        {
                reactor.tick();

                std::array< std::byte, 128 > tmp;
                auto [succ, data] = debug_comms.recv( tmp );
                if ( !succ )
                        return;
                std::ignore = em::protocol::extract_multiplexed(
                    data,
                    [&]( em::protocol::channel_type chid, std::span< std::byte const > data ) {
                            return em::protocol::multiplexed_dispatch(
                                chid, data, reactor, collector, parameters );
                    } );
        }
};

}  // namespace servio::ftest
