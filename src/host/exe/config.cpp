#include "config.hpp"

#include "host/async_cobs.h"
#include "io.pb.h"
#include "protocol.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <dimcli/cli.h>
#include <google/protobuf/util/json_util.h>

constexpr std::size_t buffer_size = 1024;

namespace host
{

boost::asio::awaitable< servio::Config >
load_config_field( boost::asio::serial_port& port, uint32_t item_number )
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

boost::asio::awaitable< std::vector< servio::Config > >
load_config( boost::asio::serial_port& port )
{
        const google::protobuf::OneofDescriptor* desc =
            servio::Config::GetDescriptor()->oneof_decl( 0 );

        std::vector< servio::Config > out;
        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );

                servio::Config cfg =
                    co_await load_config_field( port, static_cast< uint32_t >( field->number() ) );

                out.push_back( cfg );
        }
        co_return out;
}

void print_configs( const std::vector< servio::Config > out )
{
        std::cout << em::joined( out, std::string{ "\n" }, [&]( const servio::Config& cfg ) {
                return cfg.ShortDebugString();
        } ) << std::endl;
}

void print_configs_json( const std::vector< servio::Config > out )
{
        std::cout << "["
                  << em::joined(
                         em::view( out ),
                         std::string{ ",\n " },
                         [&]( const servio::Config& cfg ) {
                                 std::string tmp;
                                 google::protobuf::util::MessageToJsonString( cfg, &tmp );
                                 return tmp;
                         } )
                  << "]" << std::endl;
}

boost::asio::awaitable< void > query_cmd( boost::asio::serial_port& port, bool json )
{

        std::vector< servio::Config > out = co_await load_config( port );

        if ( json ) {
                print_configs_json( out );
        } else {
                print_configs( out );
        }
}

boost::asio::awaitable< void >
get_cmd( boost::asio::serial_port& port, const std::string& name, bool json )
{
        const google::protobuf::Descriptor* desc = servio::Config::GetDescriptor();

        const google::protobuf::FieldDescriptor* field = desc->FindFieldByName( name );

        if ( field == nullptr ) {
                std::cerr << "Failed to find config field " << name << std::endl;
                co_return;
        }

        servio::Config cfg =
            co_await load_config_field( port, static_cast< uint32_t >( field->number() ) );

        if ( json ) {
                print_configs_json( { cfg } );
        } else {
                print_configs( { cfg } );
        }
}

}  // namespace host

int main( int argc, char* argv[] )
{

        std::filesystem::path                     device;
        unsigned                                  baudrate;
        boost::asio::io_context                   context;
        std::optional< boost::asio::serial_port > opt_port;
        bool                                      json;

        Dim::Cli cli;

        cli.title( "Utility to configure servio" );
        cli.helpNoArgs();
        cli.opt( &device, "d device", "/dev/ttyUSB0" ).desc( "Device to use" );
        cli.opt( &baudrate, "b baudrate", 115200 ).desc( "Baudrate for communication" );
        cli.opt( &json, "j json", false ).desc( "Enable json output" );

        cli.command( "query" ).desc( "List all config options" );
        cli.action( [&]( Dim::Cli& ) {
                co_spawn(
                    context, host::query_cmd( opt_port.value(), json ), boost::asio::detached );
        } );

        cli.command( "get" ).desc( "Loads specific compiler option" );
        std::string field;
        cli.opt( &field, "f field" ).desc( "Name of the field to query" );
        cli.action( [&]( Dim::Cli& ) {
                co_spawn(
                    context,
                    host::get_cmd( opt_port.value(), field, json ),
                    boost::asio::detached );
        } );

        if ( !cli.parse( argc, argv ) ) {
                return cli.printError( std::cerr );
        }

        opt_port.emplace( context, device );
        opt_port->set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );

        cli.exec();

        context.run();
}
