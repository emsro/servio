#pragma once

#include "../../base.hpp"
#include "./rx_buffer.hpp"

namespace servio::drv::bits
{

struct nl_rx_container : rx_buffer
{
        uint16_t received_size = 0;
};

inline void on_rx_cplt_irq( nl_rx_container& rx, std::byte inpt )
{
        char c = (char) inpt;
        if ( c == '\n' ) {
                rx.sizes.push_back( rx.received_size );
                rx.received_size = 0;
        } else {
                rx.received_size += 1;
        }
}

}  // namespace servio::drv::bits
