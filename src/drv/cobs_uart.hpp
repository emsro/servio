#include "../plt/platform.hpp"
#include "../sntr/sentry.hpp"
#include "./bits/cobs_rx_container.h"
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
        COBS_HAL_ERROR_IRQ_ERR = 0x1,
        COBS_HAL_RX_START_ERR  = 0x2
};

class cobs_uart : public com_iface
{

public:
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

        void rx_cplt_irq( UART_HandleTypeDef* huart );
        void tx_cplt_irq( UART_HandleTypeDef* huart );
        void err_irq( UART_HandleTypeDef* huart );

        com_res recv( std::span< std::byte > data ) override;

        em::result start() override;

        em::result
        send( std::span< std::span< std::byte const > const > data, microseconds timeout ) override;

        // we tolerate overrun, dma, frame, noise error - we can recover
        static constexpr uint32_t tolerable_hal_errors =
            HAL_UART_ERROR_ORE | HAL_UART_ERROR_DMA | HAL_UART_ERROR_FE | HAL_UART_ERROR_NE;

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

inline void cobs_uart::rx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != uart_ )
                return;

        bits::on_rx_cplt_irq( rx_, rx_byte_ );

        if ( start() != em::SUCCESS )
                sentry_.set_inoperable( COBS_HAL_RX_START_ERR, "rx start" );
}

inline void cobs_uart::tx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != uart_ )
                return;
        tx_done_ = true;
}

inline void cobs_uart::err_irq( UART_HandleTypeDef* huart )
{
        if ( huart != uart_ )
                return;

        uint32_t error_status = HAL_UART_GetError( huart );
        if ( ( error_status & ~tolerable_hal_errors ) != 0 )
                sentry_.set_inoperable_set( error_status << 1, "err irq" );
        else if ( error_status != 0 )
                sentry_.set_degraded_set( error_status << 1, "err irq" );
}

inline em::result cobs_uart::start()
{
        if ( uart_ == nullptr )
                return em::ERROR;
        if ( HAL_UART_Receive_IT( uart_, reinterpret_cast< uint8_t* >( &rx_byte_ ), 1 ) != HAL_OK )
                return em::ERROR;
        return em::SUCCESS;
}

}  // namespace servio::drv
