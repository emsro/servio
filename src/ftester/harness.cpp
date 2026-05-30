#include "harness.hpp"

#include <stdexcept>
#include <termios.h>
#include <unistd.h>

harness::harness( asrtio::task_ctx& ctx, pbar::terminal_progress& bar, int peer_fd )
  : reporter_base( ctx )
  , pr_( ctx, bar )
  , peer_fd_( peer_fd )
{
}

harness::~harness()
{
        if ( peer_fd_ >= 0 )
                ::close( peer_fd_ );
}

asrtio::task< void >
harness::on_test_start( std::string_view name, uint32_t run_idx, uint32_t run_total )
{
        co_await pr_.on_test_start( name, run_idx, run_total );
        auto it = registry_.find( std::string{ name } );
        if ( it != registry_.end() ) {
                ::tcflush( peer_fd_, TCIOFLUSH );
                co_await it->second->before();
        }
}

asrtio::task< void > harness::on_test_done(
    std::string_view name,
    bool             passed,
    double           duration_ms,
    uint32_t         run_idx,
    uint32_t         run_total )
{
        co_await pr_.on_test_done( name, passed, duration_ms, run_idx, run_total );
        auto it = registry_.find( std::string{ name } );
        if ( it != registry_.end() ) {
                co_await it->second->after();
                ::tcflush( peer_fd_, TCIOFLUSH );
        }
}
