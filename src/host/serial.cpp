#include "host/serial.hpp"

#include "host/async_cobs.hpp"
#include "host/exceptions.hpp"

#include <emlabcpp/view.h>

// TODO: there might be a better way of obtaining this?
constexpr std::size_t buffer_size = 1024;

namespace host
{

boost::asio::awaitable< void > write( cobs_port& port, const servio::HostToServio& payload )
{
        servio::HostToServioPacket msg;
        msg.set_id( 0 );
        *msg.mutable_payload() = payload;

        std::size_t              size = msg.ByteSizeLong();
        std::vector< std::byte > buffer( size, std::byte{ 0 } );
        EMLABCPP_DEBUG_LOG( "Sending: ", msg.ShortDebugString() );
        if ( !msg.SerializeToArray( buffer.data(), static_cast< int >( size ) ) ) {
                throw serialize_error{ "failed to serliaze host to servio message" };
        }

        co_await port.async_write( em::view_n( buffer.data(), size ) );
}

boost::asio::awaitable< servio::ServioToHost > read( cobs_port& port )
{
        std::array< std::byte, buffer_size > reply_buffer;
        em::view< std::byte* >               deser_msg = co_await port.async_read( reply_buffer );

        if ( deser_msg.empty() ) {
                throw reply_error{ "Got an empty reply" };
        }

        servio::ServioToHostPacket reply;
        if ( !reply.ParseFromArray(
                 deser_msg.begin(), static_cast< int >( deser_msg.size() - 1 ) ) ) {
                EMLABCPP_ERROR_LOG( "Failed to parse message: ", deser_msg );
                throw parse_error{ "failed to parse servio to host message from incoming data" };
        }
        EMLABCPP_DEBUG_LOG( "Got: ", reply.ShortDebugString() );

        if ( reply.payload().has_error() ) {
                throw error_exception{ reply.payload().error() };
        }

        co_return reply.payload();
}

boost::asio::awaitable< servio::ServioToHost >
exchange( cobs_port& port, const servio::HostToServio& msg )
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
get_config_field( cobs_port& port, const google::protobuf::FieldDescriptor* field )
{
        servio::HostToServio msg;
        msg.mutable_get_config()->set_field_id( static_cast< uint32_t >( field->number() ) );

        servio::ServioToHost reply = co_await exchange( port, msg );
        if ( !reply.has_get_config() ) {
                EMLABCPP_ERROR_LOG(
                    "Expected message with get_config, instead got: ", reply.ShortDebugString() );
                throw reply_error{ "reply does not contain get_config as it should" };
        }
        co_return reply.get_config();
}

boost::asio::awaitable< void > set_config_field( cobs_port& port, const servio::Config& cfg )
{
        servio::HostToServio msg;
        *msg.mutable_set_config() = cfg;

        servio::ServioToHost reply = co_await exchange( port, msg );
        if ( !reply.has_set_config() ) {
                throw reply_error{ "reply does not contain set_config as it should" };
        }
}

boost::asio::awaitable< std::vector< servio::Config > > get_full_config( cobs_port& port )
{
        const google::protobuf::OneofDescriptor* desc =
            servio::Config::GetDescriptor()->oneof_decl( 0 );

        std::vector< servio::Config > out;
        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );

                servio::Config cfg = co_await get_config_field( port, field );

                out.push_back( cfg );
        }
        co_return out;
}

boost::asio::awaitable< servio::Property > get_property( cobs_port& port, uint32_t field_id )
{
        servio::HostToServio hts;
        hts.mutable_get_property()->set_field_id( field_id );
        servio::ServioToHost sth = co_await exchange( port, hts );
        if ( !sth.has_get_property() ) {
                EMLABCPP_ERROR_LOG(
                    "Expected message with get_property, instead got: ", sth.ShortDebugString() );
                throw reply_error{ "reply does not contain get_property as it should" };
        }
        co_return sth.get_property();
}

boost::asio::awaitable< servio::Property >
get_property( cobs_port& port, servio::Property::PldCase field_id )
{
        return get_property( port, static_cast< uint32_t >( field_id ) );
}

boost::asio::awaitable< servio::Property >
get_property( cobs_port& port, const google::protobuf::FieldDescriptor* field )
{
        return get_property( port, static_cast< uint32_t >( field->number() ) );
}

/// TODO: error checking for the getters
boost::asio::awaitable< servio::Mode > get_property_mode( cobs_port& port )
{
        servio::Property prop = co_await get_property( port, servio::Property::kMode );
        co_return prop.mode();
}

boost::asio::awaitable< float > get_property_current( cobs_port& port )
{
        servio::Property prop = co_await get_property( port, servio::Property::kCurrent );
        co_return prop.current();
}
boost::asio::awaitable< float > get_property_position( cobs_port& port )
{
        servio::Property prop = co_await get_property( port, servio::Property::kPosition );
        co_return prop.position();
}
boost::asio::awaitable< float > get_property_velocity( cobs_port& port )
{
        servio::Property prop = co_await get_property( port, servio::Property::kVelocity );
        co_return prop.velocity();
}

boost::asio::awaitable< void > set_mode( cobs_port& port, servio::Mode mode )
{
        servio::HostToServio hts;
        *hts.mutable_set_mode() = mode;

        co_await exchange( port, hts );
}

boost::asio::awaitable< void > set_mode_disengaged( cobs_port& port )
{
        servio::Mode m;
        m.mutable_disengaged();

        co_await set_mode( port, m );
}

boost::asio::awaitable< void > set_mode_position( cobs_port& port, float angle )
{
        servio::Mode m;
        m.set_position( angle );
        co_await set_mode( port, m );
}

boost::asio::awaitable< void > set_mode_velocity( cobs_port& port, float vel )
{
        servio::Mode m;
        m.set_velocity( vel );
        co_await set_mode( port, m );
}

boost::asio::awaitable< void > set_mode_current( cobs_port& port, float curr )
{
        servio::Mode m;
        m.set_current( curr );
        co_await set_mode( port, m );
}

}  // namespace host
