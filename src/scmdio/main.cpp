#include "./bflash.hpp"
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

void cobs_port_callback( CLI::App* app, io_context& io_ctx, auto& ctx_ptr, auto f )
{
        app->callback( [&io_ctx, ctx_ptr, f = std::move( f )] {
                co_spawn( io_ctx, f( ctx_ptr->port.get( io_ctx ) ), detached );
        } );
}

struct cfg_ctx
{
        bool                  json = false;
        std::string           field;
        std::string           value;
        std::filesystem::path path;
        cobs_cli              port;
};

void cfg_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< cfg_ctx >();

        auto* cfg = app.add_subcommand( "cfg", "configuration commands" );

        port_opts( *cfg, ctx->port );

        auto* query = cfg->add_subcommand( "query", "list all config options from the servo" );
        json_flag( query, ctx->json );
        cobs_port_callback( query, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) {
                return cfg_query_cmd( p, ctx->json );
        } );

        auto* get = cfg->add_subcommand( "get", "retrivies a configuration option from the servo" );
        field_option( get, ctx->field );
        json_flag( get, ctx->json );
        cobs_port_callback( get, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) {
                return cfg_get_cmd( p, ctx->field, ctx->json );
        } );

        auto* set =
            cfg->add_subcommand( "set", "sets a configuration option to value in the servo" );
        field_option( set, ctx->field );
        set->add_option( "value", ctx->value, "Value to set" );
        cobs_port_callback( set, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) {
                return cfg_set_cmd( p, ctx->field, ctx->value );
        } );

        auto* commit = cfg->add_subcommand(
            "commit", "stores the current configuration of servo in its persistent memory" );
        cobs_port_callback( commit, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) {
                return cfg_commit_cmd( p );
        } );

        auto* clear = cfg->add_subcommand( "clear", "clear latest store config from the servo" );
        cobs_port_callback( clear, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) {
                return cfg_clear_cmd( p );
        } );

        auto* load = cfg->add_subcommand( "load", "load config from a file" );
        load->add_option( "path", ctx->path, "Path to a file" );
        cobs_port_callback( load, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) {
                return cfg_load_cmd( p, ctx->path );
        } );
}

awaitable< void >
pool_cmd( io_context&, sptr< cobs_port > port, std::vector< iface::prop_key > props )
{

        while ( true ) {
                std::vector< std::string > vals;
                for ( iface::prop_key const field : props ) {
                        auto val = co_await get_property( *port, field );
                        val.visit( [&]( auto& kv ) {
                                vals.emplace_back( std::format( "{}", kv.value ) );
                        } );
                }
                std::cout << em::joined( vals, std::string{ "\t" } ) << std::endl;
        }
}

awaitable< void >
pool_cmd( io_context& context, sptr< cobs_port > port, std::vector< std::string > properties )
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

struct pool_opts
{
        std::vector< std::string > data;
        cobs_cli                   port;
};

void pool_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< pool_opts >();

        auto* pool = app.add_subcommand( "pool", "pool the servo for properties" );
        port_opts( *pool, ctx->port );
        pool->add_option( "properties", ctx->data, "properties to pool" );
        cobs_port_callback( pool, io_ctx, ctx, [&io_ctx, ctx]( sptr< cobs_port > p ) {
                return pool_cmd( io_ctx, p, ctx->data );
        } );
}

struct mode_opts
{
        float    power;
        float    current;
        float    angle;
        float    velocity;
        cobs_cli port;
};

void mode_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< mode_opts >();

        auto* mode = app.add_subcommand( "mode", "switch the servo to mode" );
        port_opts( *mode, ctx->port );

        using R = awaitable< void >;

        auto* disengaged = mode->add_subcommand( "disengaged", "disengaged mode" );
        cobs_port_callback( disengaged, io_ctx, ctx, []( sptr< cobs_port > p ) -> R {
                co_await set_mode_disengaged( *p );
        } );

        auto* pow = mode->add_subcommand( "power", "power mode" );
        pow->add_option( "power", ctx->power, "power" );
        cobs_port_callback( disengaged, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) -> R {
                co_await set_mode_power( *p, ctx->power );
        } );

        auto* current = mode->add_subcommand( "current", "current mode" );
        current->add_option( "current", ctx->current, "goal current" );
        cobs_port_callback( current, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) -> R {
                co_await set_mode_current( *p, ctx->current );
        } );

        auto* position = mode->add_subcommand( "position", "position mode" );
        position->add_option( "angle", ctx->angle, "goal angle" );
        cobs_port_callback( position, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) -> R {
                co_await set_mode_position( *p, ctx->angle );
        } );

        auto* velocity = mode->add_subcommand( "velocity", "velocity mode" );
        velocity->add_option( "velocity", ctx->velocity, "goal velocity" );
        cobs_port_callback( velocity, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) -> R {
                co_await set_mode_velocity( *p, ctx->velocity );
        } );
}

struct autotune_ctx
{
        cobs_cli port;
};

void autotune_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< autotune_ctx >();

        auto* autotune = app.add_subcommand( "autotune", "does PID autotuning" );

        port_opts( *autotune, ctx->port );

        auto* curr = autotune->add_subcommand( "current", "tune current PID" );
        cobs_port_callback( curr, io_ctx, ctx, [ctx]( sptr< cobs_port > p ) -> awaitable< void > {
                co_await pid_autotune_current( *p, 0.5F );
        } );
}

struct bflash_ctx
{
        std::filesystem::path file;
        serial_cli            port;
};

void bflash_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< bflash_ctx >();

        auto port_callback = [&]( CLI::App* cmd, auto f ) {
                cmd->callback( [&io_ctx, ctx, f = std::move( f )]() {
                        sptr< serial_stream > ss = ctx->port.get( io_ctx );
                        using spb                = boost::asio::serial_port_base;
                        ss->port.set_option( spb::parity( spb::parity::even ) );
                        ss->port.set_option( spb::character_size( 8 ) );
                        ss->port.set_option( spb::stop_bits( spb::stop_bits::one ) );

                        co_spawn( io_ctx, f( ss ), detached );
                } );
        };

        auto* bflash = app.add_subcommand( "bflash", "flash firmware via bootloader" );
        port_opts( *bflash, ctx->port );

        auto* info =
            bflash->add_subcommand( "info", "Query the system about bootloader information" )
                ->fallthrough();
        port_callback( info, []( sptr< serial_stream > ss ) -> awaitable< void > {
                co_await bflash_info( *ss, std::cout );
        } );

        auto* download =
            bflash->add_subcommand( "download", "Download the image flashed into the device" )
                ->fallthrough();
        download->add_option( "file", ctx->file, "file to download to" )->required();
        port_callback( download, [ctx]( sptr< serial_stream > ss ) -> awaitable< void > {
                std::ofstream f{ ctx->file };
                co_await bflash_download( *ss, f );
        } );

        auto* flash =
            bflash->add_subcommand( "flash", "Flash image to target device" )->fallthrough();
        flash->add_option( "file", ctx->file, "file to download to" )->required();
        port_callback( flash, [ctx]( sptr< serial_stream > ss ) -> awaitable< void > {
                std::ifstream f{ ctx->file };
                co_await bflash_flash( *ss, f );
        } );
}

}  // namespace servio::scmdio

int main( int argc, char* argv[] )
{
        spdlog::enable_backtrace( 32 );
        using namespace servio;

        boost::asio::io_context ctx;
        CLI::App                app{ "Servio utility" };

        scmdio::cfg_def( app, ctx );
        scmdio::pool_def( app, ctx );
        scmdio::mode_def( app, ctx );
        scmdio::autotune_def( app, ctx );
        scmdio::bflash_def( app, ctx );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        ctx.run();

        return 0;
}
