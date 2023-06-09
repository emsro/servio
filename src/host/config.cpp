#include "config.hpp"

#include "io.pb.h"
#include "protocol.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <emlabcpp/experimental/cobs.h>

boost::asio::awaitable< cfg_map > load_config( boost::asio::serial_port& port )
{
        cfg_map cmap;
        for ( cfg_key k : cmap.get_keys() ) {
                servio::host_to_servio msg;
                msg.mutable_get_cfg()->set_key( k );

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

                /*
                        std::array< std::byte, 1024 > data;
                        std::size_t                   n = co_await port.async_read_some(
                            boost::asio::buffer( data ), boost::asio::use_awaitable );

                        ep.insert( em::view_n( data.data(), n ) );
                        match(
                            ep.get_value(),
                            [&]( std::size_t ) {},
                            [&]( servo_to_master_variant var ) {
                                    std::ignore = var;
                            },
                            [&]( em::protocol::error_record ) {

                            } );
                    */
        }
        co_return cmap;
}
int main( int argc, char* argv[] )
{

        boost::asio::io_context  context;
        boost::asio::serial_port port{ context, argv[1] };

        co_spawn( context, load_config( port ), boost::asio::detached );

        context.run();
}
