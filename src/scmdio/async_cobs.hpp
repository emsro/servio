#include "scmdio/exceptions.hpp"

#include <array>
#include <boost/asio.hpp>
#include <emlabcpp/experimental/cobs.h>
#include <vector>

#pragma once

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

        boost::asio::awaitable< void > async_write( em::view< std::byte* > msg )
        {
                // why +16? because for small messages *2 is not enough
                std::vector< std::byte > msg_buffer( msg.size() * 2 + 16, std::byte{ 0 } );
                auto [succ, ser_msg] =
                    em::encode_cobs( msg, em::view_n( msg_buffer.data(), msg_buffer.size() - 1 ) );
                if ( !succ ) {
                        EMLABCPP_ERROR_LOG(
                            "Failed to encode cobs message of size ",
                            msg.size(),
                            " into buffer of size ",
                            msg_buffer.size() );
                        throw serialize_error{ "failed to encode cobs" };
                }

                msg_buffer[ser_msg.size()] = std::byte{ 0 };

                EMLABCPP_DEBUG_LOG( "Writing: ", ser_msg );

                co_await boost::asio::async_write(
                    port,
                    boost::asio::buffer( ser_msg.begin(), ser_msg.size() + 1 ),
                    boost::asio::use_awaitable );
        }

        std::vector< std::byte > read_buffer;

        boost::asio::awaitable< em::view< std::byte* > > async_read( em::view< std::byte* > buffer )
        {
                std::size_t n = co_await boost::asio::async_read_until(
                    port,
                    boost::asio::dynamic_buffer( read_buffer ),
                    0,
                    boost::asio::use_awaitable );
                em::view< std::byte* > dview = em::view_n( read_buffer.data(), n );
                EMLABCPP_DEBUG_LOG( "Reading: ", dview );

                auto [rsucc, deser_msg] = em::decode_cobs( dview, buffer );

                read_buffer.erase(
                    read_buffer.begin(), read_buffer.begin() + static_cast< long int >( n ) );

                if ( !rsucc )
                        throw parse_error{ "failed to parse cobs" };

                co_return deser_msg;
        }
};

}  // namespace servio::scmdio
