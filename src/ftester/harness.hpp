#pragma once

#include "pbar_reporter.hpp"
#include "transport.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct peer_hook
{
        explicit peer_hook( asrtio::task_ctx& ctx )
          : ctx_( ctx )
        {
        }

        auto& query( ecor::get_memory_resource_t tag )
        {
                return ctx_.query( tag );
        }

        auto& query( ecor::get_task_core_t tag )
        {
                return ctx_.query( tag );
        }

        virtual asrtio::task< void > before() = 0;
        virtual asrtio::task< void > after()  = 0;
        virtual ~peer_hook()                  = default;

protected:
        asrtio::task_ctx& ctx_;
};

/// Outcome of one run of a test.
struct test_run_result
{
        std::string name;
        /// Zero-based, as in the run's output directory.
        uint32_t run;
        bool     passed;
        double   duration_ms;
};

/// harness wraps pbar_reporter and adds per-test lifecycle hooks for a peer
/// device connected via a dedicated serial port.
///
/// Use add() to register a peer_hook subclass keyed by test name.
/// Before starting a test the peer serial buffer is flushed and before() is
/// awaited; after the test after() is awaited and the buffer is flushed again.
struct harness : asrtio::reporter_base
{
        /// Takes ownership of peer_fd (closes it on destruction).
        harness( asrtio::task_ctx& ctx, pbar::terminal_progress& bar, int peer_fd );

        ~harness() override;

        harness( harness const& )            = delete;
        harness& operator=( harness const& ) = delete;

        /// Register a hook for a test identified by name.
        void add( std::string name, std::unique_ptr< peer_hook > hook )
        {
                registry_.insert_or_assign( std::move( name ), std::move( hook ) );
        }

        asrtio::task< void > on_count( uint32_t total ) override
        {
                co_await pr_.on_count( total );
        }

        asrtio::task< void >
        on_test_start( std::string_view name, uint32_t run_idx, uint32_t run_total ) override;

        asrtio::task< void > on_test_done(
            std::string_view name,
            bool             passed,
            double           duration_ms,
            uint32_t         run_idx,
            uint32_t         run_total ) override;

        /// Results of the runs finished so far, in the order they finished.
        std::vector< test_run_result > const& results() const
        {
                return results_;
        }

        asrtio::task< void >
        on_diagnostic( std::string_view file, uint32_t line, std::string_view extra ) override
        {
                co_await pr_.on_diagnostic( file, line, extra );
        }

        asrtio::task< void >
        on_collect_data( std::string_view name, asrt_flat_tree const* tree ) override
        {
                co_await pr_.on_collect_data( name, tree );
        }

        asrtio::task< void >
        on_stream_data( std::string_view name, asrt::stream_schemas const& schemas ) override
        {
                co_await pr_.on_stream_data( name, schemas );
        }

private:
        asrtio::pbar_reporter                                 pr_;
        std::map< std::string, std::unique_ptr< peer_hook > > registry_;
        std::vector< test_run_result >                        results_;
        int                                                   peer_fd_;
};
