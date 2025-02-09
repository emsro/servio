#pragma once

#include <emlabcpp/static_circular_buffer.h>

namespace em = emlabcpp;

namespace servio::drv::bits
{

struct rx_buffer
{
        em::static_circular_buffer< std::byte, 256 > buffer;
        em::static_circular_buffer< uint16_t, 8 >    sizes;
};

std::tuple< bool, em::view< std::byte* > >
load_message( rx_buffer& rx, em::view< std::byte* > data );

}  // namespace servio::drv::bits
