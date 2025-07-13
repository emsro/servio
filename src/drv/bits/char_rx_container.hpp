#pragma once

#include "../../base.hpp"
#include "./rx_buffer.hpp"

namespace servio::drv::bits
{

template < char... C >
struct char_rx_container : rx_buffer
{
        uint16_t received_size = 0;
};

template < char... C >
void on_rx_cplt_irq( char_rx_container< C... >& rx, std::byte inpt )
{
        char       c       = (char) inpt;
        bool const matches = ( ( c == C ) || ... );
        if ( matches ) {
                rx.sizes.push_back( rx.received_size );
                rx.received_size = 0;
        } else {
                rx.buffer.emplace_back( inpt );
                rx.received_size += 1;
        }
}

}  // namespace servio::drv::bits
