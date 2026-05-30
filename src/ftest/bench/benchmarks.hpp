#pragma once

#include "../../core/core.hpp"
#include "../../drv/callbacks.hpp"
#include "../../drv/interfaces.hpp"
#include "../../drv/retainers.hpp"
#include "../../gov/power/power.hpp"
#include "../../plt/platform.hpp"
#include "../utest.hpp"
#include "./base.hpp"

#include <emlabcpp/enumerate.h>

namespace servio::ftest::bench
{

struct loop_frequency : utest
{
        drv::clk_iface&    clk;
        drv::period_iface& period;
        drv::pos_iface&    pos;
        drv::curr_iface&   curr;

        char const* name = "loop_frequency";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                microseconds               time_window  = 1000_ms;
                std::size_t                period_cnt   = 0;
                std::size_t                current_cnt  = 0;
                std::size_t                position_cnt = 0;
                em::min_max< std::size_t > curr_profile_size;
                curr_profile_size.min() = std::numeric_limits< std::size_t >::max();

                drv::period_cb_iface& period_cb = period.get_period_callback();
                {
                        em::defer d1 = drv::retain_callback( period );
                        em::defer d2 = drv::retain_callback( curr );
                        em::defer d3 = drv::retain_callback( pos );

                        drv::current_cb ccb{ [&]( uint32_t, std::span< uint16_t > data ) {
                                curr_profile_size = em::expand( curr_profile_size, data.size() );
                                current_cnt += 1;
                        } };
                        curr.set_current_callback( ccb );

                        drv::position_cb pocb{ [&]( uint32_t ) {
                                position_cnt += 1;
                        } };
                        pos.set_position_callback( pocb );

                        drv::period_cb pcb{ [&] {
                                period_cnt += 1;
                                period_cb.on_period_irq();
                        } };
                        period.set_period_callback( pcb );
                        co_await wait_for( *this, clk, time_window );
                }

                co_await store_metric(
                    *this,
                    "time_window",
                    std::chrono::duration_cast< sec_time >( time_window ).count(),
                    "s" );

                co_await store_as_freq( "current  cb freq", current_cnt, time_window );
                co_await store_as_freq( "position cb freq", position_cnt, time_window );

                float expected_pfreq = 10'000.0F;
                co_await store_metric( *this, "expected cb freq", expected_pfreq, "Hz" );
                float pfreq = co_await store_as_freq( "period   cb freq", period_cnt, time_window );
                co_await expect( *this, pfreq >= expected_pfreq );

                co_await store_metric(
                    *this, "measurements_min", (uint32_t) curr_profile_size.min(), "items" );
                co_await expect( *this, curr_profile_size.min() > 10 );
                co_await store_metric(
                    *this, "measurements_max", (uint32_t) curr_profile_size.max(), "items" );
        }

        ftest::task< float >
        store_as_freq( char const* sv, std::size_t counter, microseconds time_window )
        {

                float freq = static_cast< float >( counter ) /
                             std::chrono::duration_cast< sec_time >( time_window ).count();
                co_await store_metric( *this, sv, freq, "Hz" );
                co_return freq;
        }
};

struct usage : utest
{
        drv::period_iface& period;
        drv::clk_iface&    clk;

        char const* name = "usage";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );
                microseconds time_window = 1000_ms;

                std::ignore = period.stop();

                uint32_t noirq_counter = count_iterations( time_window );

                std::ignore = period.start();

                uint32_t irq_counter = count_iterations( time_window );

                co_await store_metric( *this, "cpu freq", HAL_RCC_GetSysClockFreq(), "Hz" );
                co_await store_metric(
                    *this,
                    "time_window",
                    std::chrono::duration_cast< sec_time >( time_window ).count(),
                    "s" );

                co_await store_metric( *this, "noirq_counter", noirq_counter, "ops" );
                co_await store_metric( *this, "irq_counter", irq_counter, "ops" );

                float usage_limit = 50.F;
                float usage       = 100.F - 100.F * static_cast< float >( irq_counter ) /
                                          static_cast< float >( noirq_counter );
                co_await expect( *this, usage < usage_limit );
                co_await store_metric( *this, "irq_usage", static_cast< uint32_t >( usage ), "%" );
                co_await store_metric(
                    *this, "irq_usage limit", static_cast< uint32_t >( usage_limit ), "%" );
        }

        std::size_t count_iterations( microseconds time_window )
        {
                std::size_t  counter = 0;
                microseconds end     = clk.get_us() + time_window;
                while ( clk.get_us() < end )
                        counter += 1;
                return counter;
        }
};

std::array< prof_record, 3 > PROF_BUFFER;

struct profile : utest
{
        drv::clk_iface&  clk;
        drv::curr_iface& curr;
        core::core&      cor;

        char const* name = "current_profile";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                auto&     curr_cb = curr.get_current_callback();
                em::defer d2      = drv::retain_callback( curr );
                em::defer d       = setup_poweroff( cor.gov_ );

                std::ignore = cor.gov_.activate( "power", cor.gov_mem );
                auto* p     = dynamic_cast< gov::pow::_power_gov* >( cor.gov_.active() );
                co_await expect( *this, p != nullptr );

                p->power                = p_max;
                std::size_t     write_i = 0;
                drv::current_cb ccb{ [&]( uint32_t val, std::span< uint16_t > data ) {
                        curr_cb.on_value_irq( val, data );
                        prof_record& rec = PROF_BUFFER[write_i];
                        rec.count += 1;
                        if ( data.size() > std::size( rec.sum ) )
                                return;  // TODO: report error
                        for ( auto&& [i, v] : em::enumerate( data ) )
                                rec.sum[i] += v;
                } };
                curr.set_current_callback( ccb );

                microseconds end = clk.get_us() + 10000_ms;
                uint32_t     c   = 0;

                auto s = co_await asrt::define< uint32_t, uint16_t[32] >( assm.stream, 0 );

                while ( clk.get_us() < end ) {

                        std::size_t  read_i = ( write_i + 1 ) % PROF_BUFFER.size();
                        prof_record& rec    = PROF_BUFFER[read_i];

                        co_await asrt::emit( s, rec.count, rec.sum );

                        PROF_BUFFER[read_i] = prof_record{};
                        write_i             = read_i;
                        c++;
                }

                co_await store_metric( *this, "rec_count", c );
        }
};

inline void setup_bench_tests(
    asrt::task_ctx&    ctx,
    asrt_reac_assm&    assm,
    drv::clk_iface&    clk,
    drv::pos_iface&    pos,
    drv::curr_iface&   curr,
    drv::period_iface& period,
    core::core&        cor,
    status&            res )
{
        setup_utest( ctx, assm, res, loop_frequency{ { ctx, assm }, clk, period, pos, curr } );
        setup_utest( ctx, assm, res, usage{ { ctx, assm }, period, clk } );
        setup_utest( ctx, assm, res, profile{ { ctx, assm }, clk, curr, cor } );
}

}  // namespace servio::ftest::bench
