#pragma once

#include "port.hpp"

namespace servio::scmdio
{

inline std::array< std::byte, 4 > dfu_conv( uint32_t x )
{
        std::array< std::byte, 4 > res;
        for ( uint32_t i = 0; i < 4; i++ )
                res[4 - i - 1] = static_cast< std::byte >( x >> ( i * 8 ) );
        return res;
}

inline uint32_t dfu_conv( std::span< std::byte const, 4 > data )
{
        uint32_t res = 0x00;
        for ( uint32_t i = 0; i < 4; i++ )
                res += (uint32_t) data[4 - i - 1] << i * 8;
        return res;
}

awaitable< void > dfu_info( stream_iface& port, std::ostream& os );
awaitable< void > dfu_download( stream_iface& port, std::ostream& os );
awaitable< void > dfu_upload( stream_iface& port, std::istream& is );
awaitable< void > dfu_upload_raw( stream_iface& port, std::istream& is );
awaitable< void > dfu_clear( stream_iface& port );

// Returns true if the ROM bootloader responded (session is now open), false on timeout.
awaitable< bool > dfu_try_init( stream_iface& port );

// SPECIAL Reset command (SubOpcode 0x0002): resets the device from the bootloader.
awaitable< void > dfu_reset( stream_iface& port );

}  // namespace servio::scmdio
