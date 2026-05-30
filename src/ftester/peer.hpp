#pragma once

#include "harness.hpp"

#include <unistd.h>
#include <uv.h>

/// Reads every byte arriving on the peer fd and writes it back verbatim.
struct echo_peer : peer_hook
{
        echo_peer( asrtio::task_ctx& ctx, uv_loop_t* loop, int fd )
          : peer_hook( ctx )
          , loop_( loop )
          , fd_( fd )
        {
        }

        asrtio::task< void > before() override
        {
                poll_ = new uv_poll_t{};
                uv_poll_init( loop_, poll_, fd_ );
                poll_->data = this;
                uv_poll_start( poll_, UV_READABLE, on_readable );
                co_return;
        }

        asrtio::task< void > after() override
        {
                uv_poll_stop( poll_ );
                uv_close( reinterpret_cast< uv_handle_t* >( poll_ ), []( uv_handle_t* h ) {
                        delete reinterpret_cast< uv_poll_t* >( h );
                } );
                poll_ = nullptr;
                co_return;
        }

private:
        static void on_readable( uv_poll_t* h, int status, int events )
        {
                if ( status < 0 || !( events & UV_READABLE ) )
                        return;
                auto*   self = static_cast< echo_peer* >( h->data );
                char    buf[256];
                ssize_t n;
                while ( ( n = ::read( self->fd_, buf, sizeof( buf ) ) ) > 0 )
                        ::write( self->fd_, buf, static_cast< std::size_t >( n ) );
        }

        uv_loop_t* loop_;
        int        fd_;
        uv_poll_t* poll_ = nullptr;
};

/// Reads and discards every byte arriving on the peer fd.
struct silent_peer : peer_hook
{
        silent_peer( asrtio::task_ctx& ctx, uv_loop_t* loop, int fd )
          : peer_hook( ctx )
          , loop_( loop )
          , fd_( fd )
        {
        }

        asrtio::task< void > before() override
        {
                poll_ = new uv_poll_t{};
                uv_poll_init( loop_, poll_, fd_ );
                poll_->data = this;
                uv_poll_start( poll_, UV_READABLE, on_readable );
                co_return;
        }

        asrtio::task< void > after() override
        {
                uv_poll_stop( poll_ );
                uv_close( reinterpret_cast< uv_handle_t* >( poll_ ), []( uv_handle_t* h ) {
                        delete reinterpret_cast< uv_poll_t* >( h );
                } );
                poll_ = nullptr;
                co_return;
        }

private:
        static void on_readable( uv_poll_t* h, int status, int events )
        {
                if ( status < 0 || !( events & UV_READABLE ) )
                        return;
                auto* self = static_cast< silent_peer* >( h->data );
                char  buf[256];
                while ( ::read( self->fd_, buf, sizeof( buf ) ) > 0 )
                        ;
        }

        uv_loop_t* loop_;
        int        fd_;
        uv_poll_t* poll_ = nullptr;
};
