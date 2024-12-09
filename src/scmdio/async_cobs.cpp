#include "./async_cobs.hpp"

#include "./exceptions.hpp"

#include <array>

namespace em = emlabcpp;

namespace servio::scmdio
{

boost::asio::awaitable< void > cobs_port::async_write( std::string_view msg )
{
        return this->async_write(
            em::view_n( reinterpret_cast< std::byte const* >( msg.data() ), msg.size() ) );
}

boost::asio::awaitable< void > cobs_port::async_write( em::view< std::byte const* > msg )
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
        ser_msg                    = em::view_n( ser_msg.begin(), ser_msg.size() + 1 );

        EMLABCPP_DEBUG_LOG( "Writing: ", ser_msg );

        co_await boost::asio::async_write(
            port,
            boost::asio::buffer( ser_msg.begin(), ser_msg.size() ),
            boost::asio::use_awaitable );
}

boost::asio::awaitable< em::view< std::byte* > >
cobs_port::async_read( em::view< std::byte* > buffer )
{
        std::size_t n = co_await boost::asio::async_read_until(
            port, boost::asio::dynamic_buffer( read_buffer ), 0, boost::asio::use_awaitable );
        em::view< std::byte* > dview = em::view_n( read_buffer.data(), n );
        EMLABCPP_DEBUG_LOG( "Reading: ", dview );

        auto [rsucc, deser_msg] = em::decode_cobs( dview, buffer );

        read_buffer.erase(
            read_buffer.begin(), read_buffer.begin() + static_cast< long int >( n ) );

        if ( !rsucc )
                throw parse_error{ "failed to parse cobs" };

        co_return deser_msg;
}

}  // namespace servio::scmdio
