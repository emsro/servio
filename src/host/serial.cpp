#include "host/serial.hpp"

#include "host/async_cobs.hpp"
#include "host/exceptions.hpp"

#include <emlabcpp/view.h>

// TODO: there might be a better way of obtaining this?
constexpr std::size_t buffer_size = 1024;

namespace host
{

boost::asio::awaitable< void >
write( boost::asio::serial_port& port, const servio::HostToServio& msg )
{
        std::size_t              size = msg.ByteSizeLong();
        std::vector< std::byte > buffer( size, std::byte{ 0 } );
        if ( !msg.SerializeToArray( buffer.data(), static_cast< int >( size ) ) ) {
                throw serialize_error{ "failed to serliaze host to servio message" };
        }

        co_await async_cobs_write( port, em::view_n( buffer.data(), size ) );
}

boost::asio::awaitable< servio::ServioToHost > read( boost::asio::serial_port& port )
{
        std::array< std::byte, buffer_size > reply_buffer;
        em::view< std::byte* > deser_msg = co_await async_cobs_read( port, reply_buffer );

        servio::ServioToHost reply;
        if ( !reply.ParseFromArray(
                 deser_msg.begin(), static_cast< int >( deser_msg.size() - 1 ) ) ) {
                EMLABCPP_ERROR_LOG( "Failed to parse message: ", deser_msg );
                throw parse_error{ "failed to parse servio to host message from incoming data" };
        }

        if ( reply.has_error() ) {
                throw error_exception{ reply.error() };
        }

        co_return reply;
}

boost::asio::awaitable< servio::ServioToHost >
exchange( boost::asio::serial_port& port, const servio::HostToServio& msg )
{
        co_await write( port, msg );

        try {
                servio::ServioToHost reply = co_await read( port );
                co_return reply;
        }
        catch ( error_exception& err ) {
                EMLABCPP_ERROR_LOG(
                    "For msg ",
                    msg.ShortDebugString(),
                    " got an error reply: ",
                    err.msg.ShortDebugString() );
                throw;
        }
}

boost::asio::awaitable< servio::Config >
load_config_field( boost::asio::serial_port& port, const google::protobuf::FieldDescriptor* field )
{
        servio::HostToServio msg;
        msg.mutable_get_config()->set_field_id( static_cast< uint32_t >( field->number() ) );

        servio::ServioToHost reply = co_await exchange( port, msg );
        if ( !reply.has_get_config() ) {
                throw reply_error{ "reply does not contain get_config as it should" };
        }
        co_return reply.get_config();
}

boost::asio::awaitable< void >
set_config_field( boost::asio::serial_port& port, const servio::Config& cfg )
{
        servio::HostToServio msg;
        *msg.mutable_set_config() = cfg;

        servio::ServioToHost reply = co_await exchange( port, msg );
        if ( !reply.has_set_config() ) {
                throw reply_error{ "reply does not contain set_config as it should" };
        }
}

boost::asio::awaitable< std::vector< servio::Config > >
load_full_config( boost::asio::serial_port& port )
{
        const google::protobuf::OneofDescriptor* desc =
            servio::Config::GetDescriptor()->oneof_decl( 0 );

        std::vector< servio::Config > out;
        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );

                servio::Config cfg = co_await load_config_field( port, field );

                out.push_back( cfg );
        }
        co_return out;
}

boost::asio::awaitable< servio::Property >
load_property( boost::asio::serial_port& port, uint32_t field_id )
{
        servio::HostToServio hts;
        hts.mutable_get_property()->set_field_id( field_id );
        servio::ServioToHost sth = co_await exchange( port, hts );
        if ( !sth.has_get_property() ) {
                throw reply_error{ "reply does not contain get_property as it should" };
        }
        co_return sth.get_property();
}

boost::asio::awaitable< servio::Property >
load_property( boost::asio::serial_port& port, servio::Property::PldCase field_id )
{
        return load_property( port, static_cast< uint32_t >( field_id ) );
}

boost::asio::awaitable< servio::Property >
load_property( boost::asio::serial_port& port, const google::protobuf::FieldDescriptor* field )
{
        return load_property( port, static_cast< uint32_t >( field->number() ) );
}

}  // namespace host
