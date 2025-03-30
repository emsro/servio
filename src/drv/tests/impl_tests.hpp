#pragma once

#include "../../ftest/utest.hpp"
#include "../../sntr/test_central_sentry.hpp"
#include "../callbacks.hpp"
#include "../cobs_uart.hpp"
#include "../hbridge.hpp"
#include "../interfaces.hpp"
#include "../retainers.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>

namespace servio::drv::tests
{

namespace t = em::testing;

struct cobs_uart_rx_test
{
        clk_iface& clk;

        std::string_view name = "cobs_uart_rx";

        t::coroutine< void > run( auto&, ftest::uctx& ctx )
        {
                sntr::test_central_sentry central;

                cobs_uart uart{ "test_uart", central, clk, nullptr, nullptr };

                co_await ctx.expect( central.buffer.empty() );
                uart.rx_cplt_irq( nullptr );
                co_await ctx.expect( !central.buffer.empty() );
                co_await ctx.expect( central.is_inoperable() );
        }
};

struct cobs_uart_err_test
{
        clk_iface& clk;

        std::string_view name = "cobs_uart_err";

        t::coroutine< void > run( auto&, ftest::uctx& ctx )
        {
                sntr::test_central_sentry central;
                UART_HandleTypeDef        handle;
                cobs_uart                 uart{ "test_uart", central, clk, &handle, nullptr };

                co_await ctx.expect( central.buffer.empty() );
                handle.ErrorCode = bits::uart_common_tolerable_hal_errors;
                uart.err_irq( &handle );
                co_await ctx.expect( central.buffer.size() == 1 );

                ctx.coll.set( "pos_ecode", std::get< 2 >( central.buffer[0] ) );
                co_await ctx.expect( !central.is_inoperable() );
                co_await ctx.expect(
                    std::get< 0 >( central.buffer[0] ) == sntr::test_central_sentry::DEGR );

                handle.ErrorCode = ~bits::uart_common_tolerable_hal_errors;
                uart.err_irq( &handle );
                co_await ctx.expect( central.buffer.size() == 2 );
                co_await ctx.expect( central.is_inoperable() );
                co_await ctx.expect(
                    std::get< 0 >( central.buffer[1] ) == sntr::test_central_sentry::INOP );

                co_return;
        }
};

struct hbridge_test
{
        std::string_view name = "hbridge_test";

        t::coroutine< void > run( auto&, ftest::uctx& ctx )
        {
                hbridge hb{ nullptr };
                co_await ctx.expect( hb.setup( 1, 2 ) == nullptr );

                // this sets behavior for scenario when tim == nullptr
                hb.set_power( pwr{ -1 } );
                co_await ctx.expect( hb.get_direction() == 1 );

                auto d = retain_callback( hb );

                std::size_t counter = 0;
                period_cb   pcb{ [&] {
                        counter += 1;
                } };
                hb.set_period_callback( pcb );
                co_await ctx.expect( &hb.get_period_callback() == &pcb );

                co_await ctx.expect( counter == 0 );
                hb.timer_period_irq( nullptr );
                co_await ctx.expect( counter == 1 );

                co_await ctx.expect( hb.start() == ERROR );
                co_await ctx.expect( hb.stop() == ERROR );
        }
};

// TODO: write this!
struct leds_test
{
};

// also skipped, mindfuck
struct adc_pooler_test
{
};

inline void setup_impl_tests(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    ftest::uctx&              ctx,
    clk_iface&                clk,
    status&                   res )
{
        ftest::setup_utest< cobs_uart_rx_test >( mem, reac, ctx, res, clk );
        ftest::setup_utest< cobs_uart_err_test >( mem, reac, ctx, res, clk );
        ftest::setup_utest< hbridge_test >( mem, reac, ctx, res );
}

}  // namespace servio::drv::tests
