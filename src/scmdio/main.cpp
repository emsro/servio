#include "io.pb.h"
#include "scmdio/cli.hpp"
#include "scmdio/config_cmds.hpp"
#include "scmdio/serial.hpp"

#include <emlabcpp/algorithm.h>
#include <filesystem>

namespace em = emlabcpp;

namespace scmdio
{

void json_flag( CLI::App* app, bool& flag )
{
        app->add_flag( "-j,--json", flag, "Enable json output" );
}

void field_option( CLI::App* app, std::string& field )
{
        app->add_option( "field", field, "Field name" );
}

struct cfg_opts
{
        bool        json = false;
        std::string field;
        std::string value;
};

void cfg_def(
    CLI::App&                     app,
    boost::asio::io_context&      context,
    std::unique_ptr< cobs_port >& port_ptr )
{
        auto data = std::make_shared< cfg_opts >();

        auto* cfg = app.add_subcommand( "cfg", "configuration commands" );

        auto* query = cfg->add_subcommand( "query", "list all config options from the servo" );
        json_flag( query, data->json );
        query->callback( [&port_ptr, data, &context] {
                co_spawn( context, cfg_query_cmd( *port_ptr, data->json ), boost::asio::detached );
        } );

        auto* get = cfg->add_subcommand( "get", "retrivies a configuration option from the servo" );
        field_option( get, data->field );
        json_flag( get, data->json );
        get->callback( [&port_ptr, data, &context] {
                co_spawn(
                    context,
                    cfg_get_cmd( *port_ptr, data->field, data->json ),
                    boost::asio::detached );
        } );

        auto* set =
            cfg->add_subcommand( "set", "sets a configuration option to value in the servo" );
        field_option( set, data->field );
        set->add_option( "value", data->value, "Value to set" );
        set->callback( [&port_ptr, data, &context] {
                co_spawn(
                    context,
                    cfg_set_cmd( *port_ptr, data->field, data->value ),
                    boost::asio::detached );
        } );

        auto* commit = cfg->add_subcommand(
            "commit", "stores the current configuration of servo in its persistent memory" );
        commit->callback( [&port_ptr, &context] {
                co_spawn( context, cfg_commit_cmd( *port_ptr ), boost::asio::detached );
        } );

        auto* clear = cfg->add_subcommand( "clear", "clear latest store config from the servo" );
        clear->callback( [&port_ptr, &context] {
                co_spawn( context, cfg_clear_cmd( *port_ptr ), boost::asio::detached );
        } );
}

boost::asio::awaitable< void > pool_cmd(
    boost::asio::io_context&,
    cobs_port&                                              port,
    std::vector< const google::protobuf::FieldDescriptor* > fields )
{

        while ( true ) {
                std::vector< std::string > vals;
                for ( const google::protobuf::FieldDescriptor* field : fields ) {
                        servio::Property repl = co_await get_property( port, field );

                        // TODO: this is quite a hack
                        std::string s = repl.ShortDebugString();
                        s             = s.substr( s.find( ':' ) + 1 );
                        vals.push_back( s );
                }
                std::cout << em::joined( vals, std::string{ "\t" } ) << std::endl;
        }
}

boost::asio::awaitable< void >
pool_cmd( boost::asio::io_context& context, cobs_port& port, std::vector< std::string > properties )
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

void pool_def(
    CLI::App&                     app,
    boost::asio::io_context&      context,
    std::unique_ptr< cobs_port >& port_ptr )
{
        auto data = std::make_shared< std::vector< std::string > >();

        auto* pool = app.add_subcommand( "pool", "pool the servo for properties" );
        pool->add_option( "properties", *data, "properties to pool" );
        pool->callback( [&port_ptr, data, &context] {
                co_spawn( context, pool_cmd( context, *port_ptr, *data ), boost::asio::detached );
        } );
}

struct mode_opts
{
        float current;
        float angle;
        float velocity;
};

void mode_def(
    CLI::App&                     app,
    boost::asio::io_context&      context,
    std::unique_ptr< cobs_port >& port_ptr )
{
        auto data = std::make_shared< mode_opts >();

        auto* mode = app.add_subcommand( "mode", "switch the servo to mode" );

        auto* disengaged = mode->add_subcommand( "disengaged", "disengaged mode" );
        disengaged->callback( [&port_ptr, &context] {
                co_spawn( context, set_mode_disengaged( *port_ptr ), boost::asio::detached );
        } );

        auto* current = mode->add_subcommand( "current", "current mode" );
        current->add_option( "current", data->current, "goal current" );
        current->callback( [&port_ptr, data, &context] {
                co_spawn(
                    context, set_mode_current( *port_ptr, data->current ), boost::asio::detached );
        } );

        auto* position = mode->add_subcommand( "position", "position mode" );
        position->add_option( "angle", data->angle, "goal angle" );
        position->callback( [&port_ptr, data, &context] {
                co_spawn(
                    context, set_mode_position( *port_ptr, data->angle ), boost::asio::detached );
        } );

        auto* velocity = mode->add_subcommand( "velocity", "velocity mode" );
        velocity->add_option( "velocity", data->velocity, "goal velocity" );
        velocity->callback( [&port_ptr, data, &context] {
                co_spawn(
                    context,
                    set_mode_velocity( *port_ptr, data->velocity ),
                    boost::asio::detached );
        } );
}

}  // namespace scmdio

int main( int argc, char* argv[] )
{

        CLI::App app{ "Servio utility" };

        scmdio::common_cli cli;
        cli.setup( app );

        scmdio::cfg_def( app, cli.context, cli.port_ptr );
        scmdio::pool_def( app, cli.context, cli.port_ptr );
        scmdio::mode_def( app, cli.context, cli.port_ptr );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        cli.context.run();

        return 0;
}
