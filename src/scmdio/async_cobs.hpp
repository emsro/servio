#pragma once

#include <boost/asio.hpp>
#include <emlabcpp/experimental/cobs.h>
#include <vector>

namespace em = emlabcpp;

namespace servio::scmdio
{

struct cobs_port
{

        cobs_port(
            boost::asio::io_context&     context,
            const std::filesystem::path& p,
            uint32_t                     baudrate )
          : port( context, p )
        {
                port.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );
        }

        boost::asio::serial_port port;
        std::vector< std::byte > read_buffer;

        boost::asio::awaitable< void > async_write( em::view< std::byte* > msg );

        boost::asio::awaitable< em::view< std::byte* > >
        async_read( em::view< std::byte* > buffer );
};

}  // namespace servio::scmdio
