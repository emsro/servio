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
        spdlog::debug( "writing: {}", msg );
        co_await async_write( port, boost::asio::buffer( msg ), use_awaitable );
}

awaitable< bool > serial_stream::read( std::span< std::byte > buffer )
{
        std::variant< std::size_t, std::monostate > results = co_await (
            async_read( port, boost::asio::buffer( buffer ), use_awaitable ) || timeout( 100ms ) );
        std::span< char > b2{ (char*) buffer.data(), buffer.size() };
        spdlog::debug( "reading: {}", spdlog::to_hex( buffer ) );
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

        spdlog::debug( "writing: {}", ser_msg );

        co_await async_write(
            port, boost::asio::buffer( ser_msg.begin(), ser_msg.size() ), use_awaitable );
}

namespace
{
struct match_byte
{
        std::byte b;

        template < typename Iterator >
        std::pair< Iterator, bool > operator()( Iterator begin, Iterator end ) const
        {
                auto iter = std::find_if( begin, end, [&]( char const c ) {
                        //                        spdlog::trace( "{}", +c );
                        return c == (char) b;
                } );
                if ( iter != end )
                        return { ++iter, true };
                return { iter, false };
        }
};

}  // namespace
}  // namespace servio::scmdio

namespace boost::asio
{
template <>
struct is_match_condition< servio::scmdio::match_byte > : public boost::true_type
{
};
}  // namespace boost::asio

namespace servio::scmdio
{
namespace
{

awaitable< std::span< std::byte > > _pool_buffer( auto& port, auto& read_buffer, std::byte delim )
{
        auto                   iter = std::ranges::find( read_buffer, delim );
        std::span< std::byte > dview;
        if ( iter != read_buffer.end() ) {
                spdlog::trace( "Using existing buffer" );
                dview = em::view_n(
                    read_buffer.data(),
                    (std::size_t) std::distance( read_buffer.begin(), iter ) + 1u );
        } else {
                std::vector< std::byte > expand;
                spdlog::trace( "Starting read" );
                std::size_t n = co_await async_read_until(
                    port,
                    boost::asio::dynamic_buffer( expand ),
                    match_byte{ delim },
                    use_awaitable );
                spdlog::trace( "Got: {}", std::span{ expand } );
                n += read_buffer.size();
                read_buffer.insert( read_buffer.end(), expand.begin(), expand.end() );
                dview = em::view_n( read_buffer.data(), n );
        }

        spdlog::debug( "reading: ", dview );
        co_return dview;
}
}  // namespace

awaitable< std::span< std::byte > > cobs_port::read_msg( std::span< std::byte > buffer )
{
        std::span< std::byte > dview = co_await _pool_buffer( port, read_buffer, std::byte{ 0 } );

        auto [rsucc, deser_msg] = em::decode_cobs( dview, buffer );

        read_buffer.erase( read_buffer.begin(), read_buffer.begin() + (long) dview.size() );
        if ( !rsucc )
                log_error( "Failed to parse cobs: ", dview );

        co_return deser_msg;
}

awaitable< void > char_port::write_msg( std::span< std::byte const > msg )
{
        spdlog::debug( "writing: {}", msg );

        co_await async_write( port, boost::asio::buffer( msg ), use_awaitable );

        std::array< std::byte, 1 > end = { std::byte{ ch } };
        co_await async_write( port, boost::asio::buffer( end ), use_awaitable );
}

awaitable< std::span< std::byte > > char_port::read_msg( std::span< std::byte > buffer )
{
        std::span< std::byte > dview = co_await _pool_buffer( port, read_buffer, std::byte{ ch } );
        assert( dview.size() <= buffer.size() );
        std::copy( dview.begin(), dview.end(), buffer.begin() );
        read_buffer.erase( read_buffer.begin(), read_buffer.begin() + (long) dview.size() );
        co_return dview;
}

}  // namespace servio::scmdio
