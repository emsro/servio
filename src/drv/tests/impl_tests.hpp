#pragma once

#include "drv/callbacks.hpp"
#include "drv/cobs_uart.hpp"
#include "drv/hbridge.hpp"
#include "drv/interfaces.hpp"
#include "drv/retainers.hpp"
#include "drv/tests/utest.hpp"
#include "sntr/test_central_sentry.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/coroutine.h>

namespace servio::drv::tests
{

using namespace base::literals;

struct cobs_uart_rx_test
{
        clk_interface&   clk;
        std::string_view name = "cobs_uart_rx";

        t::coroutine< void > run( auto&, collector& coll )
        {
                sntr::test_central_sentry central;

                cobs_uart uart{ "test_uart", central, clk, nullptr, nullptr };

                co_await t::expect( coll, central.buffer.empty() );
                uart.rx_cplt_irq( nullptr );
                co_await t::expect( coll, !central.buffer.empty() );
                co_await t::expect( coll, central.is_inoperable() );
        }
};

struct cobs_uart_err_test
{
        clk_interface&   clk;
        std::string_view name = "cobs_uart_err";

        t::coroutine< void > run( auto&, collector& coll )
        {
                sntr::test_central_sentry central;
                UART_HandleTypeDef        handle;
                cobs_uart                 uart{ "test_uart", central, clk, &handle, nullptr };

                co_await t::expect( coll, central.buffer.empty() );
                handle.ErrorCode = cobs_uart::tolerable_hal_errors;
                uart.err_irq( &handle );
                co_await t::expect( coll, central.buffer.size() == 1 );

                coll->set( "pos_ecode", std::get< 2 >( central.buffer[0] ) );
                co_await t::expect( coll, !central.is_inoperable() );
                co_await t::expect(
                    coll, std::get< 0 >( central.buffer[0] ) == sntr::test_central_sentry::DEGR );

                handle.ErrorCode = ~cobs_uart::tolerable_hal_errors;
                uart.err_irq( &handle );
                co_await t::expect( coll, central.buffer.size() == 2 );

                co_await t::expect( coll, central.is_inoperable() );
                co_await t::expect(
                    coll, std::get< 0 >( central.buffer[1] ) == sntr::test_central_sentry::INOP );

                co_return;
        }
};

struct hbridge_test
{
        std::string_view name = "hbridge_test";

        t::coroutine< void > run( auto&, collector& coll )
        {
                hbridge hb{ nullptr };
                co_await t::expect( coll, hb.setup( 1, 2 ) == nullptr );

                // this sets behavior for scenarion when tim == nullptr
                hb.set_power( -1 );
                co_await t::expect( coll, hb.get_direction() == 1 );

                auto d = retain_callback( hb );

                std::size_t counter = 0;
                period_cb   pcb{ [&] {
                        counter += 1;
                } };
                hb.set_period_callback( pcb );
                co_await t::expect( coll, &hb.get_period_callback() == &pcb );

                co_await t::expect( coll, counter == 0 );
                hb.timer_period_irq( nullptr );
                co_await t::expect( coll, counter == 1 );

                co_await t::expect( coll, hb.start() == em::ERROR );
                co_await t::expect( coll, hb.stop() == em::ERROR );
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
    t::collector&             coll,
    clk_interface&            clk,
    em::result&               res )

{
        setup_utest< cobs_uart_rx_test >( mem, reac, coll, res, clk );
        setup_utest< cobs_uart_err_test >( mem, reac, coll, res, clk );
        setup_utest< hbridge_test >( mem, reac, coll, res );
}

}  // namespace servio::drv::tests
