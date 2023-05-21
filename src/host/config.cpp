
#include "config.hpp"

#include "protocol.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

boost::asio::awaitable< cfg_map > load_config( boost::asio::serial_port& port, master_endpoint& ep )
{
        cfg_map cmap;
        for ( cfg_key k : cmap.get_keys() ) {
                master_to_servo_message msg = ep.serialize( get_config_request{ k } );
                co_await async_write(
                    port,
                    boost::asio::buffer( msg.data(), msg.size() ),
                    boost::asio::use_awaitable );

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
        }
        co_return cmap;
}
int main( int argc, char* argv[] )
{
        master_endpoint ep;

        boost::asio::io_context  context;
        boost::asio::serial_port port{ context, argv[1] };

        co_spawn( context, load_config( port, ep ), boost::asio::detached );

        context.run();
}
