#include "config.hpp"

#include "host/serial.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <dimcli/cli.h>
#include <google/protobuf/util/json_util.h>

namespace host
{

void print_configs( const std::vector< servio::Config > out )
{
        std::cout << em::joined( out, std::string{ "\n" }, [&]( const servio::Config& cfg ) {
                return cfg.ShortDebugString();
        } ) << std::endl;
}

void print_configs_json( const std::vector< servio::Config > out )
{
        google::protobuf::util::JsonPrintOptions ops;
        ops.preserve_proto_field_names = true;
        std::cout << "["
                  << em::joined(
                         em::view( out ),
                         std::string{ ",\n " },
                         [&]( const servio::Config& cfg ) {
                                 std::string tmp;
                                 google::protobuf::util::MessageToJsonString( cfg, &tmp, ops );
                                 return tmp;
                         } )
                  << "]" << std::endl;
}

boost::asio::awaitable< void > query_cmd( boost::asio::serial_port& port, bool json )
{
        std::vector< servio::Config > out = co_await load_full_config( port );

        if ( json ) {
                print_configs_json( out );
        } else {
                print_configs( out );
        }
}

boost::asio::awaitable< void > commit_cmd( boost::asio::serial_port& port )
{
        servio::HostToServio msg;
        msg.mutable_commit_config()->set_key( 42 );

        servio::ServioToHost reply = co_await exchange( port, msg );
        std::ignore                = reply;
        // TODO: check reply state;
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

        servio::Config cfg = co_await load_config_field( port, field );

        if ( json ) {
                print_configs_json( { cfg } );
        } else {
                print_configs( { cfg } );
        }
}

boost::asio::awaitable< void >
set_cmd( boost::asio::serial_port& port, const std::string& name, std::string value )
{
        std::cout << "setting " << name << " to: " << value << std::endl;

        const google::protobuf::Descriptor* desc = servio::Config::GetDescriptor();

        using FD        = google::protobuf::FieldDescriptor;
        const FD* field = desc->FindFieldByName( name );
        if ( field == nullptr ) {
                std::cerr << "Failed to find config field " << name << std::endl;
                co_return;
        }

        switch ( field->cpp_type() ) {
        case FD::CPPTYPE_INT32:
        case FD::CPPTYPE_INT64:
        case FD::CPPTYPE_UINT32:
        case FD::CPPTYPE_UINT64:
        case FD::CPPTYPE_FLOAT:
        case FD::CPPTYPE_DOUBLE:
                break;
        case FD::CPPTYPE_BOOL:
        case FD::CPPTYPE_ENUM:
        case FD::CPPTYPE_STRING:
                value = "\"" + value + "\"";
                break;
        case FD::CPPTYPE_MESSAGE:
                throw std::exception{};  // TODO: improve this
        }

        std::string json = "{\"" + name + "\":" + value + "}";

        servio::Config cmsg;
        auto           status = google::protobuf::util::JsonStringToMessage( json, &cmsg );
        if ( !status.ok() ) {
                throw std::exception{};  // TODO: improve
        }

        servio::HostToServio msg;
        *msg.mutable_set_config() = cmsg;

        servio::ServioToHost reply = co_await exchange( port, msg );
        std::ignore                = reply;
        // TODO: check reply status
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

        cli.command( "query" ).desc( "List all config options" );
        cli.opt( &json, "j json", false ).desc( "Enable json output" );
        cli.opt( &device, "d device", "/dev/ttyUSB0" ).desc( "Device to use" );
        cli.opt( &baudrate, "b baudrate", 115200 ).desc( "Baudrate for communication" );
        cli.action( [&]( Dim::Cli& ) {
                co_spawn(
                    context, host::query_cmd( opt_port.value(), json ), boost::asio::detached );
        } );

        cli.command( "get" ).desc( "Loads specific configuration option" );
        cli.opt( &json, "j json", false ).desc( "Enable json output" );
        cli.opt( &device, "d device", "/dev/ttyUSB0" ).desc( "Device to use" );
        cli.opt( &baudrate, "b baudrate", 115200 ).desc( "Baudrate for communication" );
        std::string field;
        cli.opt( &field, "f field" ).desc( "Name of the field to query" );
        cli.action( [&]( Dim::Cli& ) {
                co_spawn(
                    context,
                    host::get_cmd( opt_port.value(), field, json ),
                    boost::asio::detached );
        } );

        cli.command( "set" ).desc( "Sets specific configuration option" );
        cli.opt( &json, "j json", false ).desc( "Enable json output" );
        cli.opt( &device, "d device", "/dev/ttyUSB0" ).desc( "Device to use" );
        cli.opt( &baudrate, "b baudrate", 115200 ).desc( "Baudrate for communication" );
        cli.opt( &field, "f field" ).desc( "Name of the field to set" );
        std::string value;
        cli.opt( &value, "v value" ).desc( "Value to set" );
        cli.action( [&]( Dim::Cli& ) {
                co_spawn(
                    context,
                    host::set_cmd( opt_port.value(), field, value ),
                    boost::asio::detached );
        } );

        cli.command( "commit" )
            .desc( "Tells the servo to store the configuration into persistent config" );
        cli.opt( &device, "d device", "/dev/ttyUSB0" ).desc( "Device to use" );
        cli.opt( &baudrate, "b baudrate", 115200 ).desc( "Baudrate for communication" );
        cli.action( [&]( Dim::Cli& ) {
                co_spawn( context, host::commit_cmd( opt_port.value() ), boost::asio::detached );
        } );

        if ( !cli.parse( static_cast< std::size_t >( argc ), argv ) ) {
                return cli.printError( std::cerr );
        }

        opt_port.emplace( context, device );
        opt_port->set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );

        cli.exec();

        context.run();
}