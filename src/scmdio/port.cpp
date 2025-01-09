#include "./port.hpp"

#include <array>
#include <boost/asio/experimental/awaitable_operators.hpp>

namespace em = emlabcpp;

namespace servio::scmdio
{
using namespace std::chrono_literals;
using namespace boost::asio::experimental::awaitable_operators;

namespace
{
awaitable< void > timeout( auto duration )
{
        co_await boost::asio::steady_timer{ co_await boost::asio::this_coro::executor, duration }
            .async_wait( use_awaitable );
};
}  // namespace

awaitable< void > serial_stream::write( std::span< std::byte const > msg )
{
        spdlog::debug( "writing: ", msg );
        co_await async_write( port, boost::asio::buffer( msg ), use_awaitable );
}

awaitable< bool > serial_stream::read( std::span< std::byte > buffer )
{
        std::variant< std::size_t, std::monostate > results = co_await (
            async_read( port, boost::asio::buffer( buffer ), use_awaitable ) || timeout( 100ms ) );
        spdlog::debug( "reading: ", buffer );
        co_return results.index() == 0;
}

awaitable< void > cobs_port::write_msg( std::span< std::byte const > msg )
{
        // why +16? because for small messages *2 is not enough
        std::vector< std::byte > msg_buffer( msg.size() * 2 + 16, std::byte{ 0 } );
        auto [succ, ser_msg] =
            em::encode_cobs( msg, em::view_n( msg_buffer.data(), msg_buffer.size() - 1 ) );
        if ( !succ )
                log_error(
                    "Failed to encode cobs message of size {} into buffer of size {}",
                    msg.size(),
                    msg_buffer.size() );

        msg_buffer[ser_msg.size()] = std::byte{ 0 };
        ser_msg                    = em::view_n( ser_msg.begin(), ser_msg.size() + 1 );

        spdlog::debug( "writing: ", ser_msg );

        co_await async_write(
            port, boost::asio::buffer( ser_msg.begin(), ser_msg.size() ), use_awaitable );
}

awaitable< std::span< std::byte > > cobs_port::read_msg( std::span< std::byte > buffer )
{
        std::size_t n = co_await async_read_until(
            port, boost::asio::dynamic_buffer( read_buffer ), 0, use_awaitable );
        std::span< std::byte > dview = em::view_n( read_buffer.data(), n );
        spdlog::debug( "reading: ", dview );

        auto [rsucc, deser_msg] = em::decode_cobs( dview, buffer );

        read_buffer.erase(
            read_buffer.begin(), read_buffer.begin() + static_cast< long int >( n ) );

        assert( read_buffer.empty() );

        if ( !rsucc )
                log_error( "Failed to parse cobs: ", dview );

        co_return deser_msg;
}

}  // namespace servio::scmdio
