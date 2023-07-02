#include "host/serial.hpp"

#include "host/async_cobs.hpp"

#include <emlabcpp/view.h>

// TODO: there might be a better way of obtaining this?
constexpr std::size_t buffer_size = 1024;

namespace host
{

boost::asio::awaitable< servio::ServioToHost >
exchange( boost::asio::serial_port& port, const servio::HostToServio& msg )
{
        {
                std::size_t              size = msg.ByteSizeLong();
                std::vector< std::byte > buffer( size, std::byte{ 0 } );
                msg.SerializeToArray( buffer.data(), static_cast< int >( size ) );

                co_await async_cobs_write( port, em::view_n( buffer.data(), size ) );
        }

        std::array< std::byte, buffer_size > reply_buffer;
        em::view< std::byte* > deser_msg = co_await async_cobs_read( port, reply_buffer );

        servio::ServioToHost reply;
        reply.ParseFromArray( deser_msg.begin(), static_cast< int >( deser_msg.size() ) );

        co_return reply;
}

boost::asio::awaitable< servio::Config >
load_config_field( boost::asio::serial_port& port, const google::protobuf::FieldDescriptor* field )
{
        servio::HostToServio msg;
        msg.mutable_get_config()->set_key( static_cast< uint32_t >( field->number() ) );

        servio::ServioToHost reply = co_await exchange( port, msg );
        co_return reply.get_config();
        // TODO: check reply state;
}

boost::asio::awaitable< void >
set_config_field( boost::asio::serial_port& port, const servio::Config& cfg )
{
        servio::HostToServio msg;
        *msg.mutable_set_config() = cfg;

        servio::ServioToHost reply = co_await exchange( port, msg );

        // TODO: expect reply.stat == SUCCES
}

}  // namespace host
