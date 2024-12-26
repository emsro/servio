#pragma once

#include <boost/asio.hpp>
#include <emlabcpp/experimental/cobs.h>
#include <vector>

namespace em = emlabcpp;

namespace servio::scmdio
{

struct port_iface
{
        virtual boost::asio::awaitable< void > async_write( em::view< std::byte const* > msg ) = 0;

        virtual boost::asio::awaitable< em::view< std::byte* > >
        async_read( em::view< std::byte* > buffer ) = 0;

        virtual ~port_iface() = default;
};

template < typename T >
using sptr = std::shared_ptr< T >;

struct cobs_port : port_iface
{

        cobs_port(
            boost::asio::io_context&     context,
            std::filesystem::path const& p,
            uint32_t                     baudrate )
          : port( context, p )
        {
                port.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );
        }

        boost::asio::serial_port port;
        std::vector< std::byte > read_buffer;

        boost::asio::awaitable< void > async_write( em::view< std::byte const* > msg ) override;

        boost::asio::awaitable< em::view< std::byte* > >
        async_read( em::view< std::byte* > buffer ) override;
};

}  // namespace servio::scmdio
