#include "base/base.hpp"
#include "drv/interfaces.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/parameters.h>
#include <emlabcpp/experimental/testing/reactor.h>

#pragma once

namespace servio::ftest
{

using namespace base::literals;

struct testing_system
{
        drv::com_iface& debug_comms;

        em::testing::reactor    reactor;
        em::testing::collector  collector;
        em::testing::parameters parameters;

        auto send_cb()
        {
                return [this]( uint16_t channel, const auto& data ) {
                        auto msg = em::protocol::serialize_multiplexed( channel, data );
                        return debug_comms.send( msg, 100_ms );
                };
        }

        testing_system( drv::com_iface& debug_comms, std::string_view name )
          : debug_comms( debug_comms )
          , reactor( em::testing::core_channel, name, send_cb() )
          , collector( em::testing::collect_channel, send_cb() )
          , parameters( em::testing::params_channel, send_cb() )
        {
        }

        void tick()
        {
                reactor.tick();

                std::array< std::byte, 128 > tmp;
                auto [succ, data] = debug_comms.load_message( tmp );
                if ( !succ )
                        return;
                std::ignore = em::protocol::extract_multiplexed(
                    data,
                    [&]( em::protocol::channel_type chid, std::span< const std::byte > data ) {
                            return em::protocol::multiplexed_dispatch(
                                chid, data, reactor, collector, parameters );
                    } );
        }
};

}  // namespace servio::ftest
