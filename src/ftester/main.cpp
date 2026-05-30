#include "final_receiver.hpp"
#include "harness.hpp"
#include "log_sink.hpp"
#include "peer.hpp"
#include "real_fs.hpp"
#include "run_session.hpp"

#include <CLI/CLI.hpp>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <sstream>

using namespace std::literals::chrono_literals;

namespace
{
pbar::terminal_progress  g_bar;
pbar::terminal_progress* g_active_bar = nullptr;
asrt_log_level           g_log_level  = ASRT_LOG_ERROR;
std::ostream*            g_log_file   = nullptr;

}  // namespace

extern "C" {
void asrt_log( enum asrt_log_level level, char const* module, char const* fmt, ... )
{
        va_list args;
        va_start( args, fmt );
        asrtio::log_sink_write( level, g_log_level, module, fmt, args, g_log_file, g_active_bar );
        va_end( args );
}
}

int main( int argc, char* argv[] )
{
        using namespace asrtio;
        uv_loop_t* loop = uv_default_loop();
        std::optional< complete_arena_connect_result< task< void >, final_receiver > > t;
        asrt::malloc_free_memory_resource                                              mem_res;
        real_fs                                                                        rfs;
        null_fs                                                                        nfs;
        task_ctx     ctx{ mem_res };
        arena        ar{ ctx, mem_res };
        steady_clock clk;
        uv_idle_t    idle;
        CLI::App     app{ "servio ftest runner" };
        argv = app.ensure_utf8( argv );

        int verbosity = 0;
        app.add_flag( "-v,--verbose", verbosity, "Verbosity: -v = info, -vv = debug" );

        uint32_t    timeout_ms = 30000U;
        std::string params_file;
        std::string output_dir;
        app.add_option( "--timeout", timeout_ms, "Timeout in milliseconds" );
        app.add_option( "--params", params_file, "Path to JSON param config file" );
        app.add_option( "--output", output_dir, "Output directory for test data files" );

        serial_config ser_cfg;
        app.add_option( "--port,-d", ser_cfg.path, "Serial device path (e.g. /dev/ttyUSB0)" )
            ->required();
        app.add_option( "--baud,-b", ser_cfg.baud, "Baud rate" )->default_val( 230400U );

        serial_config peer_cfg;
        app.add_option( "--peer-port,-p", peer_cfg.path, "Peer device serial port" )->required();
        app.add_option( "--peer-baud", peer_cfg.baud, "Peer device baud rate" )
            ->default_val( 230400U );

        CLI11_PARSE( app, argc, argv );

        if ( verbosity >= 2 )
                g_log_level = ASRT_LOG_DEBUG;
        else if ( verbosity == 1 )
                g_log_level = ASRT_LOG_INFO;
        else
                g_log_level = ASRT_LOG_ERROR;

        std::optional< file_writer > log_writer;
        if ( !output_dir.empty() ) {
                rfs.create_directories( output_dir );
                log_writer.emplace(
                    rfs.open_write( std::filesystem::path{ output_dir } / "ftest.log" ) );
                g_log_file = &log_writer->stream();
        }

        if ( output_dir.empty() )
                ASRT_INF_LOG( "asrtio", "No --output specified; data files will not be written" );
        auto params = std::make_unique< param_config >();
        if ( !params_file.empty() ) {
                params = param_config_from_file( params_file );
                if ( !params ) {
                        std::fprintf( stderr, "Failed to load param config\n" );
                        std::exit( 1 );
                }
        }

        std::string peer_errmsg;
        int         peer_fd = asrtio::open_serial_port( peer_cfg, peer_errmsg );
        if ( peer_fd < 0 ) {
                std::fprintf( stderr, "Failed to open peer port: %s\n", peer_errmsg.c_str() );
                std::exit( 1 );
        }
        harness h{ ctx, g_bar, peer_fd };
        h.add( "comms_echo", std::make_unique< echo_peer >( ctx, loop, peer_fd ) );
        h.add( "comms_timeout", std::make_unique< echo_peer >( ctx, loop, peer_fd ) );

        auto timeout = std::chrono::milliseconds{ timeout_ms };
        t.emplace(
            ar,
            run_serial(
                ctx,
                ar,
                clk,
                loop,
                ser_cfg,
                timeout,
                std::move( params ),
                output_dir.empty() ? static_cast< output_fs& >( nfs ) : rfs,
                output_dir,
                h ),
            final_receiver{ &idle, &g_active_bar } );
        g_active_bar = &g_bar;

        idle.data = &ctx;
        uv_idle_init( loop, &idle );
        uv_idle_start( &idle, []( uv_idle_t* handle ) {
                auto& ctx = *static_cast< task_ctx* >( handle->data );
                ctx.tick();
        } );

        if ( t )
                t->start();

        uv_run( loop, UV_RUN_DEFAULT );
        uv_loop_close( loop );
        g_bar.finish();

        g_log_file = nullptr;
        log_writer.reset();

        return 0;
}
