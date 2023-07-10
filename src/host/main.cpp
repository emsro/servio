#include "host/cli.hpp"
#include "host/config_cmds.hpp"
#include "host/serial.hpp"
#include "io.pb.h"

#include <emlabcpp/algorithm.h>
#include <filesystem>

args::Flag json_flag( args::Group& g )
{
        return args::Flag( g, "json", "Enable json output", { 'j', "json" } );
}

args::Positional< std::string > field_pos( args::Group& g )
{
        return args::Positional< std::string >(
            g, "field", "Name of the field to get", args::Options::Required );
}

bool cfg_commands(
    boost::asio::io_context&                       context,
    boost::asio::serial_port&                      port,
    std::function< bool( args::ArgumentParser& ) > cont )
{
        args::ArgumentParser parser( "cfg args" );
        args::Group          commands( parser, "commands" );
        args::Command        query(
            commands, "query", "list all config options", [&]( args::Subparser& parser ) {
                    args::Flag json = json_flag( parser );
                    parser.Parse();

                    co_spawn( context, host::query_cmd( port, json ), boost::asio::detached );
            } );
        args::Command get(
            commands, "get", "loads specific configuration option", [&]( args::Subparser& parser ) {
                    args::Flag                      json  = json_flag( parser );
                    args::Positional< std::string > field = field_pos( parser );
                    parser.Parse();

                    co_spawn(
                        context, host::get_cmd( port, field.Get(), json ), boost::asio::detached );
            } );
        args::Command set(
            commands, "set", "sets specific configuration option", [&]( args::Subparser& parser ) {
                    args::Positional< std::string > field = field_pos( parser );
                    args::Positional< std::string > value(
                        parser, "value", "Value to set", args::Options::Required );
                    parser.Parse();

                    co_spawn(
                        context,
                        host::set_cmd( port, field.Get(), value.Get() ),
                        boost::asio::detached );
            } );
        args::Command commit(
            commands,
            "commit",
            "tell the servo to store it's current configuration into persistent memory",
            [&]( args::Subparser& parser ) {
                    parser.Parse();

                    co_spawn( context, host::commit_cmd( port ), boost::asio::detached );
            } );
        args::Command clear(
            commands,
            "clear",
            "tells the servo to clear latest store config",
            [&]( args::Subparser& parser ) {
                    parser.Parse();

                    co_spawn( context, host::clear_cmd( port ), boost::asio::detached );
            } );

        return cont( parser );
}

boost::asio::awaitable< void > pool_cmd(
    boost::asio::io_context&,
    boost::asio::serial_port&                               port,
    std::vector< const google::protobuf::FieldDescriptor* > fields )
{

        while ( true ) {
                std::vector< std::string > vals;
                for ( const google::protobuf::FieldDescriptor* field : fields ) {
                        servio::Property repl = co_await host::load_property( port, field );

                        // TODO: this is quite a hack
                        std::string s = repl.ShortDebugString();
                        s             = s.substr( s.find( ':' ) + 1 );
                        vals.push_back( s );
                }
                std::cout << em::joined( vals, std::string{ "," } ) << std::endl;
        }
}

boost::asio::awaitable< void > pool_cmd(
    boost::asio::io_context&   context,
    boost::asio::serial_port&  port,
    std::vector< std::string > properties )
{
        if ( properties.empty() ) {
                std::cout << "got an empty property list, not pooling" << std::endl;
                co_return;
        }

        std::vector< const google::protobuf::FieldDescriptor* > fields;
        for ( const std::string& prop : properties ) {
                const google::protobuf::Descriptor* desc = servio::Property::GetDescriptor();

                const google::protobuf::FieldDescriptor* field = desc->FindFieldByName( prop );
                if ( field == nullptr ) {
                        std::cerr << "Failed to find property: " << prop << std::endl;
                        co_return;
                }
                fields.push_back( field );
        }
        co_await pool_cmd( context, port, fields );
}

bool pool_commands(
    boost::asio::io_context&                       context,
    boost::asio::serial_port&                      port,
    std::function< bool( args::ArgumentParser& ) > cont )
{
        args::ArgumentParser               parser( "pool the servo for properties" );
        args::ValueFlagList< std::string > properties(
            parser,
            "properties",
            "Properties to pool for, use repeatedly for multiple properties",
            { 'p', "property" } );

        bool succ = cont( parser );
        if ( !succ ) {
                return false;
        }

        co_spawn( context, pool_cmd( context, port, properties.Get() ), boost::asio::detached );

        return true;
}

int main( int argc, char* argv[] )
{

        using command_sig = bool(
            boost::asio::io_context&,
            boost::asio::serial_port&,
            std::function< bool( args::ArgumentParser& ) > );

        using command_type = std::function< command_sig >;

        std::unordered_map< std::string, command_type > cmd_map{
            { "cfg", cfg_commands }, { "pool", pool_commands } };

        std::vector< std::string > cmd_vec =
            em::map_f< std::vector< std::string > >( cmd_map, []( const auto& pair ) {
                    return pair.first;
            } );
        std::string cmd_list = em::joined( cmd_vec, std::string{ "," } );

        args::ArgumentParser parser( "Servio utility" );

        args::HelpFlag help( parser, "help", "Display this help menu", { 'h', "help" } );
        args::ValueFlag< std::filesystem::path > device(
            parser, "device", "Device to use", { 'd', "device" }, "/dev/ttyUSB0" );
        args::ValueFlag< unsigned > baudrate(
            parser, "baudrate", "Baudrate for communication", { 'b', "baudrate" }, 115200 );
        args::MapPositional< std::string, command_type > command(
            parser, "command", "Commands to execute: " + cmd_list, cmd_map );
        command.KickOut( true );

        parser.Prog( argv[0] );
        std::vector< std::string > vec( argv + 1, argv + argc );
        em::view                   dview = em::data_view( vec );
        const std::string*         next  = host::parse_args( parser, dview );

        if ( !command ) {
                return 0;
        }

        boost::asio::io_context  context;
        boost::asio::serial_port port( context, device.Get() );
        port.set_option( boost::asio::serial_port_base::baud_rate( baudrate.Get() ) );

        auto cont_f = [&]( args::ArgumentParser& parser ) -> bool {
                parser.Prog( argv[0] );
                return host::parse_args( parser, em::view( next, dview.end() ) ) != nullptr;
        };

        bool succ = args::get( command )( context, port, cont_f );
        if ( !succ ) {
                return 1;
        }

        context.run();

        return 0;
}
