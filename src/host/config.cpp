#include "config.hpp"

#include "io.pb.h"
#include "protocol.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <emlabcpp/experimental/cobs.h>

boost::asio::awaitable< void > load_config( boost::asio::serial_port& port )
{
        const google::protobuf::OneofDescriptor* desc =
            servio::Config::GetDescriptor()->oneof_decl( 0 );

        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );

                EMLABCPP_INFO_LOG( "Querying ", field->full_name() );

                servio::HostToServio msg;
                msg.mutable_get_config()->set_key( static_cast< uint32_t >( field->number() ) );

                {
                        std::array< std::byte, 1024 > buffer;
                        std::size_t                   size = msg.ByteSizeLong();
                        msg.SerializeToArray( buffer.data(), static_cast< int >( size ) );

                        std::array< std::byte, 1024 > msg_buffer;
                        auto [succ, ser_msg] =
                            em::encode_cobs( em::view_n( buffer.data(), size ), msg_buffer );

                        co_await async_write(
                            port,
                            boost::asio::buffer( ser_msg.begin(), ser_msg.size() ),
                            boost::asio::use_awaitable );
                }

                std::vector< uint8_t > reply_raw_buffer;
                std::size_t            n = co_await async_read_until(
                    port,
                    boost::asio::dynamic_buffer( reply_raw_buffer ),
                    0,
                    boost::asio::use_awaitable );

                std::array< std::byte, 1024 > reply_buffer;
                auto [rsucc, deser_msg] = em::decode_cobs(
                    em::view_n( reinterpret_cast< std::byte* >( reply_raw_buffer.data() ), n ),
                    reply_buffer );

                servio::ServioToHost reply;
                reply.ParseFromArray( deser_msg.begin(), static_cast< int >( deser_msg.size() ) );
                EMLABCPP_INFO_LOG( "Got ", reply.DebugString() );
        }
}
int main( int argc, char* argv[] )
{

        boost::asio::io_context  context;
        boost::asio::serial_port port{ context, argv[1] };

        co_spawn( context, load_config( port ), boost::asio::detached );

        context.run();
}
