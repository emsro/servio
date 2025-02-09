#pragma once

#include "../../base.hpp"
#include "./rx_buffer.hpp"

#include <emlabcpp/experimental/cobs.h>

namespace em = emlabcpp;

namespace servio::drv::bits
{

struct cobs_rx_container : rx_buffer
{
        uint16_t         received_size = 0;
        em::cobs_decoder cobs_dec;
};

inline void on_rx_cplt_irq( cobs_rx_container& rx, std::byte inpt )
{
        if ( inpt == std::byte{ 0 } ) {
                rx.sizes.push_back( rx.received_size - 1 );
                rx.received_size = 0;
        } else if ( rx.received_size == 0 ) {
                rx.cobs_dec = em::cobs_decoder{ inpt };
                rx.received_size += 1;
        } else {
                opt< std::byte > const b = rx.cobs_dec.iter( inpt );
                if ( b.has_value() ) {
                        rx.received_size += 1;
                        rx.buffer.push_back( *b );
                }
        }
}

}  // namespace servio::drv::bits
