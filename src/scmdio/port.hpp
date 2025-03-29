#pragma once

#include "base.hpp"
#include "spdlog/fmt/bin_to_hex.h"

#include <emlabcpp/experimental/cobs.h>
#include <vector>

namespace em = emlabcpp;

namespace servio::scmdio
{

struct stream_iface
{
        virtual awaitable< void > write( std::span< std::byte const > msg ) = 0;

        awaitable< void > write( std::byte b )
        {
                std::byte data[] = { b };
                co_await write( data );
        }

        virtual awaitable< bool > read( std::span< std::byte > buffer ) = 0;

        awaitable< opt< std::byte > > read()
        {
                std::byte        data[1];
                opt< std::byte > res;
                if ( co_await read( data ) )
                        res = data[0];
                co_return res;
        }

        virtual ~stream_iface() = default;
};

inline void flush_port( boost::asio::serial_port& p )
{
        int const res = ::tcflush( p.lowest_layer().native_handle(), TCIOFLUSH );
        if ( res != 0 )
                spdlog::error( "Failed to flush serial buffer" );
}

inline std::filesystem::path const& check_path( std::filesystem::path const& p )
{
        if ( !std::filesystem::exists( p ) )
                throw std::runtime_error( "Path " + p.string() + " does not exist" );
        return p;
}

struct serial_stream : stream_iface
{
        serial_stream( io_context& io_ctx, std::filesystem::path const& p, uint32_t baudrate )
          : port( io_ctx, check_path( p ) )
        {
                port.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );
                flush_port( port );
        }

        boost::asio::serial_port port;
        std::vector< std::byte > read_buffer;

        awaitable< void > write( std::span< std::byte const > msg ) override;

        awaitable< bool > read( std::span< std::byte > buffer ) override;
};

struct port_iface
{
        virtual awaitable< void > write_msg( std::span< std::byte const > msg ) = 0;

        virtual awaitable< std::span< std::byte > > read_msg( std::span< std::byte > buffer ) = 0;

        virtual ~port_iface() = default;
};

struct cobs_port : port_iface

{
        cobs_port( io_context& context, std::filesystem::path const& p, uint32_t baudrate )
          : port( context, check_path( p ) )
        {
                port.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );
                flush_port( port );
        }

        boost::asio::serial_port port;
        std::vector< std::byte > read_buffer;

        awaitable< void > write_msg( std::span< std::byte const > msg ) override;

        awaitable< std::span< std::byte > > read_msg( std::span< std::byte > buffer ) override;
};

struct char_port : port_iface
{
        static constexpr char ch = '\0';

        char_port( io_context& context, std::filesystem::path const& p, uint32_t baudrate )
          : port( context, check_path( p ) )
        {
                port.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );
                flush_port( port );
        }

        boost::asio::serial_port port;
        std::vector< std::byte > read_buffer;

        awaitable< void > write_msg( std::span< std::byte const > msg ) override;

        awaitable< std::span< std::byte > > read_msg( std::span< std::byte > buffer ) override;
};

}  // namespace servio::scmdio
