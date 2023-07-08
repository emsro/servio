#include "config.hpp"

#include "host/cli.hpp"
#include "host/serial.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
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
                                 std::ignore =
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
        msg.mutable_commit_config()->set_nothing( 42 );

        servio::ServioToHost reply = co_await exchange( port, msg );
        std::ignore                = reply;
        // TODO: check reply state;
}

boost::asio::awaitable< void > clear_cmd( boost::asio::serial_port& port )
{
        servio::HostToServio msg;
        msg.mutable_clear_config()->set_nothing( 42 );
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

        if ( field->is_repeated() ) {
                std::cerr << "Config field is repeated, which is not supported" << std::endl;
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

        boost::asio::io_context                   context;
        std::optional< boost::asio::serial_port > opt_port;

        args::ArgumentParser parser( "Utility to configure servio" );

        args::Group arguments(
            "arguments", args::Group::Validators::DontCare, args::Options::Global );

        args::HelpFlag help( arguments, "help", "Display this help menu", { 'h', "help" } );
        args::ValueFlag< std::filesystem::path > device(
            arguments, "device", "Device to use", { 'd', "device" }, "/dev/ttyUSB0" );
        args::ValueFlag< unsigned > baudrate(
            arguments, "baudrate", "Baudrate for communication", { 'b', "baudrate" }, 115200 );

        auto setup_port = [&] {
                opt_port.emplace( context, device.Get() );
                opt_port->set_option( boost::asio::serial_port_base::baud_rate( baudrate.Get() ) );
        };

        args::GlobalOptions globals( parser, arguments );

        args::Group   commands( parser, "commands" );
        args::Command query(
            commands, "query", "list all config options", [&]( args::Subparser& parser ) {
                    args::Flag json( parser, "json", "Enable json output", { 'j', "json" } );
                    parser.Parse();

                    setup_port();
                    co_spawn(
                        context, host::query_cmd( opt_port.value(), json ), boost::asio::detached );
            } );
        args::Command get(
            commands, "get", "loads specific configuration option", [&]( args::Subparser& parser ) {
                    args::Flag json( parser, "json", "Enable json output", { 'j', "json" } );
                    args::Positional< std::string > field(
                        parser, "field", "Name of the field to get", args::Options::Required );
                    parser.Parse();

                    setup_port();
                    co_spawn(
                        context,
                        host::get_cmd( opt_port.value(), field.Get(), json ),
                        boost::asio::detached );
            } );
        args::Command set(
            commands, "set", "sets specific configuration option", [&]( args::Subparser& parser ) {
                    args::Positional< std::string > field(
                        parser, "field", "Name of the field to set", args::Options::Required );
                    args::Positional< std::string > value(
                        parser, "value", "Value to set", args::Options::Required );
                    parser.Parse();

                    setup_port();
                    co_spawn(
                        context,
                        host::set_cmd( opt_port.value(), field.Get(), value.Get() ),
                        boost::asio::detached );
            } );
        args::Command commit(
            commands,
            "commit",
            "tell the servo to store it's current configuration into persistent memory",
            [&]( args::Subparser& parser ) {
                    parser.Parse();

                    setup_port();
                    co_spawn(
                        context, host::commit_cmd( opt_port.value() ), boost::asio::detached );
            } );
        args::Command clear(
            commands,
            "clear",
            "tells the servo to clear latest store config",
            [&]( args::Subparser& parser ) {
                    parser.Parse();

                    setup_port();
                    co_spawn( context, host::clear_cmd( opt_port.value() ), boost::asio::detached );
            } );

        int res = host::parse_cli( parser, argc, argv );

        context.run();

        return res;
}
