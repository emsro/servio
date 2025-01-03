#include "./cli.hpp"
#include "./config_cmds.hpp"
#include "./field_util.hpp"
#include "./pid_autotune_cmd.hpp"
#include "./serial.hpp"

#include <emlabcpp/algorithm.h>
#include <filesystem>

namespace em = emlabcpp;

namespace servio::scmdio
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
        bool                  json = false;
        std::string           field;
        std::string           value;
        std::filesystem::path path;
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

        auto* load = cfg->add_subcommand( "load", "load config from a file" );
        load->add_option( "path", data->path, "Path to a file" );
        load->callback( [&port_ptr, data, &context] {
                co_spawn( context, cfg_load_cmd( *port_ptr, data->path ), boost::asio::detached );
        } );
}

boost::asio::awaitable< void >
pool_cmd( boost::asio::io_context&, cobs_port& port, std::vector< iface::prop_key > props )
{

        while ( true ) {
                std::vector< std::string > vals;
                for ( iface::prop_key const field : props ) {
                        auto val = co_await get_property( port, field );
                        val.visit( [&]( auto& kv ) {
                                vals.emplace_back( std::format( "{}", kv.value ) );
                        } );
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

        std::vector< iface::prop_key > fields;
        for ( std::string const& prop : properties ) {
                auto s = iface::prop_key::from_string( prop );
                if ( !s ) {
                        std::cerr << "Failed to find property: " << prop << std::endl;
                        co_return;
                }
                fields.push_back( std::move( *s ) );
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
        float power;
        float current;
        float angle;
        float velocity;
};

void mode_def( CLI::App& app, boost::asio::io_context& ctx, std::unique_ptr< cobs_port >& port_ptr )
{
        auto data = std::make_shared< mode_opts >();

        auto* mode = app.add_subcommand( "mode", "switch the servo to mode" );

        auto* disengaged = mode->add_subcommand( "disengaged", "disengaged mode" );
        disengaged->callback( [&port_ptr, &ctx] {
                co_spawn( ctx, set_mode_disengaged( *port_ptr ), boost::asio::detached );
        } );

        auto* pow = mode->add_subcommand( "power", "power mode" );
        pow->add_option( "power", data->power, "power" );
        pow->callback( [&port_ptr, data, &ctx] {
                co_spawn( ctx, set_mode_power( *port_ptr, data->power ), boost::asio::detached );
        } );

        auto* current = mode->add_subcommand( "current", "current mode" );
        current->add_option( "current", data->current, "goal current" );
        current->callback( [&port_ptr, data, &ctx] {
                co_spawn(
                    ctx, set_mode_current( *port_ptr, data->current ), boost::asio::detached );
        } );

        auto* position = mode->add_subcommand( "position", "position mode" );
        position->add_option( "angle", data->angle, "goal angle" );
        position->callback( [&port_ptr, data, &ctx] {
                co_spawn( ctx, set_mode_position( *port_ptr, data->angle ), boost::asio::detached );
        } );

        auto* velocity = mode->add_subcommand( "velocity", "velocity mode" );
        velocity->add_option( "velocity", data->velocity, "goal velocity" );
        velocity->callback( [&port_ptr, data, &ctx] {
                co_spawn(
                    ctx, set_mode_velocity( *port_ptr, data->velocity ), boost::asio::detached );
        } );
}

void autotune_def(
    CLI::App&                     app,
    boost::asio::io_context&      ctx,
    std::unique_ptr< cobs_port >& port_ptr )
{
        auto* autotune = app.add_subcommand( "autotune", "does PID autotuning" );

        auto* curr = autotune->add_subcommand( "current", "tune current PID" );
        curr->callback( [&port_ptr, &ctx] {
                co_spawn( ctx, pid_autotune_current( *port_ptr, 0.5F ), boost::asio::detached );
        } );
}

}  // namespace servio::scmdio

int main( int argc, char* argv[] )
{
        using namespace servio;

        CLI::App app{ "Servio utility" };

        scmdio::common_cli cli;
        cli.setup( app );

        scmdio::cfg_def( app, cli.context, cli.port_ptr );
        scmdio::pool_def( app, cli.context, cli.port_ptr );
        scmdio::mode_def( app, cli.context, cli.port_ptr );
        scmdio::autotune_def( app, cli.context, cli.port_ptr );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        cli.context.run();

        return 0;
}
