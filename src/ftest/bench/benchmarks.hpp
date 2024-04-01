#pragma once

#include "core/core.hpp"
#include "drv/callbacks.hpp"
#include "drv/interfaces.hpp"
#include "drv/retainers.hpp"
#include "ftest/bench/base.hpp"
#include "ftest/utest.hpp"
#include "platform.hpp"

#include <emlabcpp/enumerate.h>

namespace servio::ftest::bench
{

struct loop_frequency
{
        drv::clk_iface&    clk;
        drv::period_iface& period;
        drv::pos_iface&    pos;
        drv::curr_iface&   curr;

        std::string_view name = "loop_frequency";

        t::coroutine< void > run( auto& mem, uctx& ctx )
        {
                base::microseconds         time_window  = 1000_ms;
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
                        drv::wait_for( clk, time_window );
                }

                co_await store_metric(
                    mem,
                    ctx,
                    "time_window",
                    std::chrono::duration_cast< base::sec_time >( time_window ),
                    "s" );

                co_await store_as_freq( mem, ctx, "current  cb freq", current_cnt, time_window );
                co_await store_as_freq( mem, ctx, "position cb freq", position_cnt, time_window );

                float expected_pfreq = 10'000.0F;
                co_await store_metric( mem, ctx, "expected cb freq", expected_pfreq, "Hz" );
                float pfreq =
                    co_await store_as_freq( mem, ctx, "period   cb freq", period_cnt, time_window );
                co_await t::expect( ctx.coll, pfreq >= expected_pfreq );

                co_await store_metric(
                    mem, ctx, "measurements_min", curr_profile_size.min(), "items" );
                co_await t::expect( ctx.coll, curr_profile_size.min() > 10 );
                co_await store_metric(
                    mem, ctx, "measurements_max", curr_profile_size.max(), "items" );
        }

        t::coroutine< float > store_as_freq(
            em::pmr::memory_resource& mem,
            uctx&                     coll,
            const std::string_view    sv,
            std::size_t               counter,
            base::microseconds        time_window )
        {

                float freq = static_cast< float >( counter ) /
                             std::chrono::duration_cast< base::sec_time >( time_window ).count();
                co_await store_metric( mem, coll, sv, freq, "Hz" );
                co_return freq;
        }
};

struct usage
{
        drv::period_iface& period;
        drv::clk_iface&    clk;

        std::string_view name = "usage";

        t::coroutine< void > run( auto& mem, uctx& ctx )
        {
                base::microseconds time_window = 1000_ms;

                std::ignore = period.stop();

                std::size_t noirq_counter = count_iterations( time_window );

                std::ignore = period.start();

                std::size_t irq_counter = count_iterations( time_window );

                co_await store_metric( mem, ctx, "cpu freq", HAL_RCC_GetSysClockFreq(), "Hz" );
                co_await store_metric(
                    mem,
                    ctx,
                    "time_window",
                    std::chrono::duration_cast< base::sec_time >( time_window ),
                    "s" );

                co_await store_metric( mem, ctx, "noirq_counter", noirq_counter, "ops" );
                co_await store_metric( mem, ctx, "irq_counter", irq_counter, "ops" );

                float usage_limit = 50.F;
                float usage       = 100.F - 100.F * static_cast< float >( irq_counter ) /
                                          static_cast< float >( noirq_counter );
                co_await t::expect( ctx.coll, usage < usage_limit );
                co_await store_metric(
                    mem, ctx, "irq_usage", static_cast< uint32_t >( usage ), "%" );
                co_await store_metric(
                    mem, ctx, "irq_usage limit", static_cast< uint32_t >( usage_limit ), "%" );
        }

        std::size_t count_iterations( base::microseconds time_window )
        {
                std::size_t        counter = 0;
                base::microseconds end     = clk.get_us() + time_window;
                while ( clk.get_us() < end )
                        counter += 1;
                return counter;
        }
};

std::array< prof_record, 3 > PROF_BUFFER;

struct profile
{
        drv::clk_iface&  clk;
        drv::curr_iface& curr;
        core::core&      cor;

        std::string_view name = "current_profile";

        t::coroutine< void > run( auto& mem, uctx& ctx )
        {
                auto&     curr_cb = curr.get_current_callback();
                em::defer d2      = drv::retain_callback( curr );
                em::defer d       = setup_poweroff( cor.ctl );

                cor.ctl.switch_to_power_control( -std::numeric_limits< int16_t >::max() / 2 );
                std::size_t     write_i = 0;
                drv::current_cb ccb{ [&]( uint32_t val, std::span< uint16_t > data ) {
                        curr_cb.on_value_irq( val, data );
                        prof_record& rec = PROF_BUFFER[write_i];
                        rec.count += 1;
                        if ( data.size() > rec.sum.size() )
                                return;  // TODO: report error
                        for ( auto&& [i, v] : em::enumerate( data ) )
                                rec.sum[i] += v;
                        for ( auto&& [i, v] : em::enumerate( data ) )
                                rec.max[i] = std::max( rec.max[i], v );
                        for ( auto&& [i, v] : em::enumerate( data ) )
                                rec.min[i] = std::min( rec.min[i], v );
                } };
                curr.set_current_callback( ccb );

                base::microseconds end = clk.get_us() + 1000_ms;
                std::size_t        c   = 0;

                while ( clk.get_us() < end ) {

                        std::size_t read_i = ( write_i + 1 ) % PROF_BUFFER.size();

                        auto msg =
                            em::protocol::handler< prof_record >::serialize( PROF_BUFFER[read_i] );

                        co_await ctx.expect( ctx.record( msg ) == em::SUCCESS );

                        PROF_BUFFER[read_i] = prof_record{};
                        write_i             = read_i;
                        c++;
                }

                co_await store_metric( mem, ctx, "rec_count", c );
        }
};

inline void setup_bench_tests(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    uctx&                     ctx,
    drv::clk_iface&           clk,
    drv::pos_iface&           pos,
    drv::curr_iface&          curr,
    drv::period_iface&        period,
    core::core&               cor,
    em::result&               res )
{
        setup_utest< loop_frequency >( mem, reac, ctx, res, clk, period, pos, curr );
        setup_utest< usage >( mem, reac, ctx, res, period, clk );
        setup_utest< profile >( mem, reac, ctx, res, clk, curr, cor );
}

}  // namespace servio::ftest::bench
