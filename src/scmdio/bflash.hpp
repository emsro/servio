#pragma once

#include "port.hpp"

namespace servio::scmdio
{

inline std::array< std::byte, 4 > bflash_conv( uint32_t x )
{
        std::array< std::byte, 4 > res;
        for ( uint32_t i = 0; i < 4; i++ )
                res[4 - i - 1] = static_cast< std::byte >( x >> ( i * 8 ) );
        return res;
}

inline uint32_t bflash_conv( std::span< std::byte const, 4 > data )
{
        uint32_t res = 0x00;
        for ( uint32_t i = 0; i < 4; i++ )
                res += (uint32_t) data[4 - i - 1] << i * 8;
        return res;
}

awaitable< void > bflash_info( stream_iface& port, std::ostream& os );
awaitable< void > bflash_download( stream_iface& port, std::ostream& os );
awaitable< void > bflash_upload( stream_iface& port, std::istream& is );

}  // namespace servio::scmdio
