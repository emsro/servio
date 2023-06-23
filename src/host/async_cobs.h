#include <array>
#include <boost/asio.hpp>
#include <emlabcpp/experimental/cobs.h>
#include <vector>

#pragma once

namespace host
{

template < typename Port >
boost::asio::awaitable< void > async_cobs_write( Port& port, em::view< std::byte* > msg )
{
        std::vector< std::byte > msg_buffer( msg.size() * 2, std::byte{ 0 } );
        auto [succ, ser_msg] = em::encode_cobs( msg, em::data_view( msg_buffer ) );

        msg_buffer[ser_msg.size()] = std::byte{ 0 };  // TODO: well this is unsafe

        co_await async_write(
            port,
            boost::asio::buffer( ser_msg.begin(), ser_msg.size() + 1 ),
            boost::asio::use_awaitable );
}

template < typename Port >
boost::asio::awaitable< em::view< std::byte* > >
async_cobs_read( Port& port, em::view< std::byte* > buffer )
{
        std::vector< uint8_t > reply_raw_buffer;
        std::size_t            n = co_await async_read_until(
            port, boost::asio::dynamic_buffer( reply_raw_buffer ), 0, boost::asio::use_awaitable );

        auto [rsucc, deser_msg] = em::decode_cobs(
            em::view_n( reinterpret_cast< std::byte* >( reply_raw_buffer.data() ), n ), buffer );

        co_return deser_msg;
}

}  // namespace host
