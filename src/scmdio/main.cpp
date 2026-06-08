#include "./cli.hpp"
#include "./config_cmds.hpp"
#include "./dfu_flash.hpp"
#include "./field_util.hpp"
#include "./pid_autotune_cmd.hpp"
#include "./preset.hpp"
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

void handle_eptr( std::exception_ptr eptr )
{
        try {
                if ( eptr )
                        std::rethrow_exception( eptr );
        }
        catch ( std::exception const& e ) {
                spdlog::dump_backtrace();
                spdlog::error( "Caught an exception: {}", e.what() );
        }
        catch ( ... ) {
                spdlog::dump_backtrace();
                spdlog::error( "Caught an unknown exception" );
        }
}

void port_callback( CLI::App* app, io_context& io_ctx, auto& ctx_ptr, auto f )
{
        app->callback( [&io_ctx, ctx_ptr, f = std::move( f )] {
                co_spawn( io_ctx, f( ctx_ptr->port.get( io_ctx ) ), handle_eptr );
        } );
}

struct cfg_ctx
{
        bool                  json = false;
        std::string           field;
        std::string           value;
        std::filesystem::path path;
        char_cli              port;
};

void cfg_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< cfg_ctx >();

        auto* cfg = app.add_subcommand( "cfg", "configuration commands" )->fallthrough();
        cfg->require_subcommand( 1 );

        port_opts( *cfg, ctx->port );

        auto* query = cfg->add_subcommand( "query", "list all config options from the servo" );
        json_flag( query, ctx->json );
        port_callback( query, io_ctx, ctx, [ctx]( sptr< char_port > p ) {
                return cfg_query_cmd( p, ctx->json );
        } );

        auto* get = cfg->add_subcommand( "get", "retrivies a configuration option from the servo" );
        field_option( get, ctx->field );
        json_flag( get, ctx->json );
        port_callback( get, io_ctx, ctx, [ctx]( sptr< char_port > p ) {
                return cfg_get_cmd( p, ctx->field, ctx->json );
        } );

        auto* set =
            cfg->add_subcommand( "set", "sets a configuration option to value in the servo" );
        field_option( set, ctx->field );
        set->add_option( "value", ctx->value, "Value to set" );
        port_callback( set, io_ctx, ctx, [ctx]( sptr< char_port > p ) {
                nlohmann::json j = nlohmann::json::parse( ctx->value );
                return cfg_set_cmd( p, ctx->field, std::move( j ) );
        } );

        auto* commit = cfg->add_subcommand(
            "commit", "stores the current configuration of servo in its persistent memory" );
        port_callback( commit, io_ctx, ctx, [ctx]( sptr< char_port > p ) {
                return cfg_commit_cmd( p );
        } );

        auto* clear = cfg->add_subcommand( "clear", "clear latest store config from the servo" );
        port_callback( clear, io_ctx, ctx, [ctx]( sptr< char_port > p ) {
                return cfg_clear_cmd( p );
        } );

        auto* load = cfg->add_subcommand( "load", "load config from a file" );
        load->add_option( "path", ctx->path, "Path to a file" );
        port_callback( load, io_ctx, ctx, [ctx]( sptr< char_port > p ) {
                return cfg_load_cmd( p, ctx->path );
        } );
}

awaitable< void >
pool_cmd( io_context&, sptr< char_port > port, std::vector< std::string > const& props )
{
        if ( props.empty() ) {
                std::cout << "got an empty property list, not pooling" << std::endl;
                co_return;
        }

        while ( true ) {
                std::vector< std::string > vals;
                for ( std::string const& field : props ) {
                        auto val = co_await get_property( *port, field );
                        vals.emplace_back( val.dump() );
                }
                std::cout << em::joined( vals, std::string{ "\t" } ) << std::endl;
        }
}

struct pool_opts
{
        std::vector< std::string > data;
        char_cli                   port;
};

void pool_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< pool_opts >();

        auto* pool = app.add_subcommand( "pool", "pool the servo for properties" );
        port_opts( *pool, ctx->port );
        pool->add_option( "properties", ctx->data, "properties to pool" );
        port_callback( pool, io_ctx, ctx, [&io_ctx, ctx]( sptr< char_port > p ) {
                return pool_cmd( io_ctx, p, ctx->data );
        } );
}

struct govctl_opts
{
        std::string governor;
        char_cli    port;
};

void govctl_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< govctl_opts >();

        auto* gov = app.add_subcommand( "govctl", "control servo governor" );
        gov->require_subcommand( 1 );
        port_opts( *gov, ctx->port );

        using R        = awaitable< void >;
        auto* activate = gov->add_subcommand( "activate", "activate governor" );
        activate->add_option( "gov", ctx->governor, "governor to activate" )->required();
        port_callback( activate, io_ctx, ctx, [ctx]( sptr< char_port > p ) -> R {
                co_await govctl_activate( *p, ctx->governor );
        } );

        auto* deactivate =
            gov->add_subcommand( "deactivate", "deactivate currently active governor" );
        port_callback( deactivate, io_ctx, ctx, [ctx]( sptr< char_port > p ) -> R {
                co_await govctl_deactivate( *p );
        } );

        auto* active = gov->add_subcommand( "active", "get currently active governor" );
        port_callback( active, io_ctx, ctx, [ctx]( sptr< char_port > p ) -> R {
                auto s = co_await govctl_active( *p );
                std::cout << "gov: " << s << std::endl;
        } );

        auto* list = gov->add_subcommand( "list", "list governors" );
        port_callback( list, io_ctx, ctx, [ctx]( sptr< char_port > p ) -> R {
                for ( std::size_t i = 0;; ++i ) {
                        auto s = co_await govctl_list( *p, i );
                        if ( !s )
                                break;
                        std::cout << "gov: " << *s << std::endl;
                }
        } );
}

struct gov_opts
{
        std::string governor;
        char_cli    port;
};

void gov_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< gov_opts >();

        auto* gov = app.add_subcommand( "gov", "interact with a specific governor" );
        gov->add_option( "governor", ctx->governor, "governor to interact with" )->required();
        port_opts( *gov, ctx->port );

        port_callback( gov, io_ctx, ctx, [ctx, &app]( sptr< char_port > p ) -> awaitable< void > {
                auto rest = app.remaining( true );
                co_await do_gov( *p, ctx->governor, { rest }, {} );
        } );
}

struct autotune_ctx
{
        char_cli port;
};

void autotune_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< autotune_ctx >();

        auto* autotune = app.add_subcommand( "autotune", "does PID autotuning" );

        port_opts( *autotune, ctx->port );

        auto* curr = autotune->add_subcommand( "current", "tune current PID" );
        port_callback( curr, io_ctx, ctx, [ctx]( sptr< char_port > p ) -> awaitable< void > {
                co_await pid_autotune_current( *p, 0.5F );
        } );
}

struct dfu_flash_ctx
{
        std::filesystem::path file;
        serial_cli            port;
};

struct dfu_enter_ctx
{
        char_cli port;
};

void dfu_def( CLI::App& app, io_context& io_ctx )
{
        auto flash_ctx = std::make_shared< dfu_flash_ctx >();
        auto enter_ctx = std::make_shared< dfu_enter_ctx >();

        auto serial_cb = [&]( CLI::App* cmd, auto f ) {
                cmd->callback( [&io_ctx, flash_ctx, f = std::move( f )]() {
                        sptr< serial_stream > ss = flash_ctx->port.get( io_ctx );
                        using spb                = boost::asio::serial_port_base;
                        ss->port.set_option( spb::parity( spb::parity::even ) );
                        ss->port.set_option( spb::character_size( 8 ) );
                        ss->port.set_option( spb::stop_bits( spb::stop_bits::one ) );

                        co_spawn( io_ctx, f( ss ), detached );
                } );
        };

        auto* dfu = app.add_subcommand( "dfu", "DFU bootloader commands" )->require_subcommand( 1 );

        auto* enter = dfu->add_subcommand(
            "enter", "Send dfu command to the firmware to enter the ROM bootloader" );
        port_opts( *enter, enter_ctx->port );
        port_callback( enter, io_ctx, enter_ctx, []( sptr< char_port > p ) -> awaitable< void > {
                co_await exchg( *p, "dfu" );
                spdlog::info( "Device is entering bootloader mode" );
        } );

        auto* info = dfu->add_subcommand( "info", "Query the system about bootloader information" );
        port_opts( *info, flash_ctx->port );
        serial_cb( info, []( sptr< serial_stream > ss ) -> awaitable< void > {
                co_await dfu_flash_info( *ss, std::cout );
        } );

        auto* download =
            dfu->add_subcommand( "download", "Download the image flashed into the device" );
        port_opts( *download, flash_ctx->port );
        download->add_option( "file", flash_ctx->file, "file to download to" )->required();
        serial_cb( download, [flash_ctx]( sptr< serial_stream > ss ) -> awaitable< void > {
                std::ofstream f{ flash_ctx->file };
                co_await dfu_flash_download( *ss, f );
        } );

        auto* flash = dfu->add_subcommand( "flash", "Flash image to target device" );
        port_opts( *flash, flash_ctx->port );
        flash
            ->add_option( "file", flash_ctx->file, "file to flash into the device, has to be .bin" )
            ->required();
        serial_cb( flash, [flash_ctx]( sptr< serial_stream > ss ) -> awaitable< void > {
                std::ifstream f{ flash_ctx->file };
                co_await dfu_flash_flash( *ss, f );
        } );

        auto* clear = dfu->add_subcommand( "clear", "Clear the device" );
        port_opts( *clear, flash_ctx->port );
        serial_cb( clear, []( sptr< serial_stream > ss ) -> awaitable< void > {
                co_await dfu_flash_clear( *ss );
        } );
}

struct preset_ctx
{
        std::filesystem::path preset_folder;
        char_cli              port;
};

void preset_def( CLI::App& app, io_context& io_ctx )
{
        auto ctx = std::make_shared< preset_ctx >();

        auto* preset = app.add_subcommand( "preset", "Manage presets" )
                           ->fallthrough()
                           ->require_subcommand( 1 );
        port_opts( *preset, ctx->port );

        auto* load = preset->add_subcommand( "load", "Load a preset" );
        load->add_option( "preset", ctx->preset_folder, "Preset name" )->required();
        port_callback( load, io_ctx, ctx, [ctx]( sptr< char_port > p ) -> awaitable< void > {
                co_await load_preset_cmd( *p, ctx->preset_folder );
        } );
}

}  // namespace servio::scmdio

int main( int argc, char* argv[] )
{
        spdlog::enable_backtrace( 32 );
        using namespace servio;

        boost::asio::io_context ctx;
        CLI::App                app{ "Servio utility" };

        scmdio::verbose_opt( app );

        scmdio::cfg_def( app, ctx );
        scmdio::pool_def( app, ctx );
        scmdio::govctl_def( app, ctx );
        // scmdio::autotune_def( app, ctx ); XXX: finish
        scmdio::dfu_def( app, ctx );
        scmdio::preset_def( app, ctx );

        app.require_subcommand( 1 );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        ctx.run();

        return 0;
}
