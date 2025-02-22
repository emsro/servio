#include "../plt/platform.hpp"
#include "./bits/cobs_rx_container.hpp"
#include "./bits/uart.hpp"
#include "./interfaces.hpp"

#include <emlabcpp/experimental/cobs.h>
#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/result.h>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

enum cobs_error_codes
{
        COBS_HAL_ERROR_IRQ_ERR = 0x3,
        COBS_HAL_RX_START_ERR  = 0x4
};

struct cobs_uart final : public com_iface
{
        cobs_uart(
            char const*                 id,
            sntr::central_sentry_iface& central,
            clk_iface&                  clk,
            UART_HandleTypeDef*         uart,
            DMA_HandleTypeDef*          tx_dma );

        cobs_uart( cobs_uart const& )            = delete;
        cobs_uart( cobs_uart&& )                 = delete;
        cobs_uart& operator=( cobs_uart const& ) = delete;
        cobs_uart& operator=( cobs_uart&& )      = delete;

        void rx_cplt_irq( UART_HandleTypeDef* huart )
        {
                return bits::uart_rx_cplt_irq< COBS_HAL_RX_START_ERR >(
                    huart, uart_, *this, rx_, rx_byte_, sentry_ );
        }

        void tx_cplt_irq( UART_HandleTypeDef* huart )
        {
                if ( huart != uart_ )
                        return;
                tx_done_ = true;
        }

        void err_irq( UART_HandleTypeDef* huart )
        {
                return bits::uart_err_irq( huart, uart_, sentry_ );
        }

        com_res recv( std::span< std::byte > data ) override
        {
                auto [res, used] = bits::load_message( rx_, data );
                return { res, used };
        }

        em::result start() override
        {
                return bits::uart_start_it( uart_, rx_byte_ );
        }

        em::result send( send_data_t data, microseconds timeout ) override;

private:
        sntr::sentry sentry_;
        clk_iface&   clk_;

        UART_HandleTypeDef* uart_   = nullptr;
        DMA_HandleTypeDef*  tx_dma_ = nullptr;

        bool volatile tx_done_ = true;
        std::array< std::byte, 1024 > tx_buffer_;

        std::byte               rx_byte_;
        bits::cobs_rx_container rx_;
};

inline cobs_uart::cobs_uart(
    char const*                 id,
    sntr::central_sentry_iface& central,
    clk_iface&                  clk,
    UART_HandleTypeDef*         uart,
    DMA_HandleTypeDef*          tx_dma )
  : sentry_( id, central )
  , clk_( clk )
  , uart_( uart )
  , tx_dma_( tx_dma )
{
}

}  // namespace servio::drv
