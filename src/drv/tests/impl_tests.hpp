#pragma once

#include "../../ftest/utest.hpp"
#include "../../sntr/test_central_sentry.hpp"
#include "../callbacks.hpp"
#include "../clock.hpp"
#include "../cobs_uart.hpp"
#include "../hbridge.hpp"
#include "../interfaces.hpp"
#include "../quad_encoder.hpp"
#include "../retainers.hpp"

namespace servio::drv::tests
{

struct cobs_uart_rx_test : ftest::utest
{
        clk_iface& clk;

        char const* name = "cobs_uart_rx";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                sntr::test_central_sentry central;

                cobs_uart uart{ "test_uart", central, clk, nullptr, nullptr };

                co_await expect( *this, central.buffer.empty() );
                uart.rx_cplt_irq( nullptr );
                co_await expect( *this, !central.buffer.empty() );
                co_await expect( *this, central.is_inoperable() );
        }
};

struct cobs_uart_err_test : ftest::utest
{
        clk_iface& clk;

        char const* name = "cobs_uart_err";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                sntr::test_central_sentry central;
                UART_HandleTypeDef        handle;
                cobs_uart                 uart{ "test_uart", central, clk, &handle, nullptr };

                co_await expect( *this, central.buffer.empty() );
                handle.ErrorCode = bits::uart_common_tolerable_hal_errors;
                uart.err_irq( &handle );
                co_await expect( *this, central.buffer.size() == 1 );

                co_await store_metric( *this, "pos_ecode", std::get< 2 >( central.buffer[0] ) );
                co_await expect( *this, !central.is_inoperable() );
                co_await expect(
                    *this, std::get< 0 >( central.buffer[0] ) == sntr::test_central_sentry::DEGR );

                handle.ErrorCode = ~bits::uart_common_tolerable_hal_errors;
                uart.err_irq( &handle );
                co_await expect( *this, central.buffer.size() == 2 );
                co_await expect( *this, central.is_inoperable() );
                co_await expect(
                    *this, std::get< 0 >( central.buffer[1] ) == sntr::test_central_sentry::INOP );
        }
};

struct hbridge_test : ftest::utest
{
        char const* name = "hbridge_test";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                hbridge hb{ nullptr };
                co_await expect( *this, hb.setup( 1, 2 ) == nullptr );

                // this sets behavior for scenario when tim == nullptr
                hb.set_power( pwr{ -1 } );
                co_await expect( *this, hb.get_direction() == 1 );

                auto d = retain_callback( hb );

                std::size_t counter = 0;
                period_cb   pcb{ [&] {
                        counter += 1;
                } };
                hb.set_period_callback( pcb );
                co_await expect( *this, &hb.get_period_callback() == &pcb );

                co_await expect( *this, counter == 0 );
                hb.timer_period_irq( nullptr );
                co_await expect( *this, counter == 1 );

                co_await expect( *this, hb.start() == status::error );
                co_await expect( *this, hb.stop() == status::error );
        }
};

// TODO: write this!
struct leds_test
{
};

struct quad_encoder_test : ftest::utest
{
        char const* name = "quad_encoder_test";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                TIM_TypeDef       regs{};
                TIM_HandleTypeDef h{};
                h.Instance    = &regs;
                h.Init.Period = 959;
                quad_encoder enc{ h };
                co_await expect( *this, enc.get_position_range().max() == 960U );

                std::size_t calls = 0;
                position_cb cb{ [&]( uint32_t ) {
                        calls += 1;
                } };
                enc.set_position_callback( cb );
                for ( std::size_t i = 0; i < 9; i++ )
                        enc.on_tick_irq();
                co_await expect( *this, calls == 2 );
        }
};

struct clock_overflow_test : ftest::utest
{
        char const* name = "clock_overflow_test";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                TIM_TypeDef       regs{};
                TIM_HandleTypeDef h{};
                h.Instance    = &regs;
                h.Init.Period = 4'999;
                clock clk{ h };
                regs.CNT = 4'999;
                co_await expect( *this, clk.get_us() == microseconds{ 4'999 } );
                clk.timer_period_irq( &h );
                regs.CNT = 0;
                co_await expect( *this, clk.get_us() == microseconds{ 5'000 } );
        }
};

// also skipped, mindfuck
struct adc_pooler_test
{
};

inline void
setup_impl_tests( asrt::task_ctx& ctx, asrt_reac_assm& assm, clk_iface& clk, status& res )
{
        ftest::setup_utest( ctx, assm, res, cobs_uart_rx_test{ { ctx, assm }, clk } );
        ftest::setup_utest( ctx, assm, res, cobs_uart_err_test{ { ctx, assm }, clk } );
        ftest::setup_utest( ctx, assm, res, hbridge_test{ { ctx, assm } } );
        ftest::setup_utest( ctx, assm, res, quad_encoder_test{ { ctx, assm } } );
        ftest::setup_utest( ctx, assm, res, clock_overflow_test{ { ctx, assm } } );
}

}  // namespace servio::drv::tests
