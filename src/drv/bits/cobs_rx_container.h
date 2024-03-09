#pragma once

#include <emlabcpp/experimental/cobs.h>
#include <emlabcpp/static_circular_buffer.h>

namespace em = emlabcpp;

namespace servio::drv::bits
{

struct cobs_rx_container
{
        uint16_t                                     received_size = 0;
        em::cobs_decoder                             cobs_dec;
        em::static_circular_buffer< std::byte, 256 > buffer;
        em::static_circular_buffer< uint16_t, 8 >    sizes;
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
                const std::optional< std::byte > b = rx.cobs_dec.iter( inpt );
                if ( b.has_value() ) {
                        rx.received_size += 1;
                        rx.buffer.push_back( *b );
                }
        }
}

inline std::tuple< bool, em::view< std::byte* > >
load_message( cobs_rx_container& rx, em::view< std::byte* > data )
{
        if ( rx.sizes.empty() )
                return { true, {} };

        // this condition can block it indefinetly right?
        if ( rx.sizes.front() > data.size() )
                return { false, {} };

        const uint16_t size  = rx.sizes.take_front();
        em::view       dview = em::view_n( data.begin(), size );
        for ( std::byte& b : dview )
                b = rx.buffer.take_front();

        return { true, dview };
}

}  // namespace servio::drv::bits
