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

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                sntr::test_central_sentry central;

                cobs_uart uart{ "test_uart", central, clk, nullptr, nullptr };

                rec.expect( central.buffer.empty() );
                uart.rx_cplt_irq( nullptr );
                rec.expect( !central.buffer.empty() );
                rec.expect( central.is_inoperable() );

                co_return;
        }
};

struct cobs_uart_err_test
{
        clk_interface&   clk;
        std::string_view name = "cobs_uart_err";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                sntr::test_central_sentry central;
                UART_HandleTypeDef        handle;
                cobs_uart                 uart{ "test_uart", central, clk, &handle, nullptr };

                rec.expect( central.buffer.empty() );
                handle.ErrorCode = cobs_uart::tolerable_hal_errors;
                uart.err_irq( &handle );
                if ( central.buffer.size() != 1 ) {
                        rec.fail();
                        co_return;
                }
                rec.expect( !central.is_inoperable() );
                rec.expect( std::get< 0 >( central.buffer[0] ) == sntr::test_central_sentry::DEGR );

                handle.ErrorCode = ~cobs_uart::tolerable_hal_errors;
                uart.err_irq( &handle );
                co_await rec.expect( central.buffer.size() != 2 );

                rec.expect( central.is_inoperable() );
                rec.expect( std::get< 0 >( central.buffer[1] ) == sntr::test_central_sentry::INOP );

                co_return;
        }
};

struct hbridge_test
{
        std::string_view name = "hbridge_test";

        em::testing::test_coroutine run( auto&, em::testing::record& rec )
        {
                hbridge hb{ nullptr };
                rec.expect( hb.setup( 1, 2 ) == nullptr );

                hb.set_power( -1 );
                rec.expect( hb.get_direction() == 0 );

                auto d = retain_callback( hb );

                std::size_t counter = 0;
                period_cb   pcb{ [&] {
                        counter += 1;
                } };
                hb.set_period_callback( pcb );
                rec.expect( &hb.get_period_callback() == &pcb );

                co_await rec.expect( counter == 0 );
                hb.timer_period_irq( nullptr );
                co_await rec.expect( counter == 1 );

                rec.expect( hb.start() == em::ERROR );
                rec.expect( hb.stop() == em::ERROR );
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
    em::testing::reactor&     reac,
    clk_interface&            clk,
    em::result&               res )

{
        setup_utest< cobs_uart_rx_test >( mem, reac, res, clk );
        setup_utest< cobs_uart_err_test >( mem, reac, res, clk );
        setup_utest< hbridge_test >( mem, reac, res );
}

}  // namespace servio::drv::tests
