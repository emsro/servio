#include "./rx_buffer.hpp"

namespace servio::drv::bits
{

std::tuple< bool, em::view< std::byte* > >
load_message( rx_buffer& rx, em::view< std::byte* > data )
{
        if ( rx.sizes.empty() )
                return { true, {} };

        // dupl with cobs_rx_container
        // this condition can block it indefinetly right?
        if ( rx.sizes.front() > data.size() )
                return { false, {} };

        uint16_t const size  = rx.sizes.take_front();
        em::view       dview = em::view_n( data.begin(), size );
        for ( std::byte& b : dview )
                b = rx.buffer.take_front();

        return { true, dview };
}

}  // namespace servio::drv::bits
