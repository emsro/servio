#include "config.hpp"

#include "host/async_cobs.h"
#include "io.pb.h"
#include "protocol.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

constexpr std::size_t buffer_size = 1024;

namespace host
{

boost::asio::awaitable< servio::Config >
query_config( boost::asio::serial_port& port, uint32_t item_number )
{
        servio::HostToServio msg;
        msg.mutable_get_config()->set_key( item_number );

        {
                std::array< std::byte, buffer_size > buffer;
                std::size_t                          size = msg.ByteSizeLong();
                msg.SerializeToArray( buffer.data(), static_cast< int >( size ) );

                co_await async_cobs_write( port, em::view_n( buffer.data(), size ) );
        }

        std::array< std::byte, buffer_size > reply_buffer;
        em::view< std::byte* > deser_msg = co_await async_cobs_read( port, reply_buffer );

        servio::ServioToHost reply;
        reply.ParseFromArray( deser_msg.begin(), static_cast< int >( deser_msg.size() ) );
        co_return reply.get_config();
}

boost::asio::awaitable< void > load_config( boost::asio::serial_port& port )
{
        const google::protobuf::OneofDescriptor* desc =
            servio::Config::GetDescriptor()->oneof_decl( 0 );

        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );

                servio::Config cfg =
                    co_await query_config( port, static_cast< uint32_t >( field->number() ) );
                EMLABCPP_INFO_LOG( "Got ", cfg.ShortDebugString() );
        }
}
}  // namespace host

int main( int argc, char* argv[] )
{

        boost::asio::io_context  context;
        boost::asio::serial_port port{ context, argv[1] };
        port.set_option( boost::asio::serial_port_base::baud_rate( 115200 ) );
        co_spawn( context, host::load_config( port ), boost::asio::detached );

        context.run();
}
