
#include "./bits/char_rx_container.hpp"
#include "./bits/uart.hpp"
#include "./interfaces.hpp"

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

enum nl_error_codes
{
        NL_HAL_ERROR_IRQ_ERR = 0x1,
        NL_HAL_RX_START_ERR  = 0x2
};

template < char... Delims >
struct char_uart final : public com_iface
{
        static constexpr char delims[] = { Delims... };

        char_uart(
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

        char_uart( char_uart const& )            = delete;
        char_uart( char_uart&& )                 = delete;
        char_uart& operator=( char_uart const& ) = delete;
        char_uart& operator=( char_uart&& )      = delete;

        void rx_cplt_irq( UART_HandleTypeDef* huart )
        {
                return bits::uart_rx_cplt_irq< NL_HAL_RX_START_ERR >(
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

        status start() override
        {
                return bits::uart_start_it( uart_, rx_byte_ );
        }

        status send( send_data_t data, microseconds timeout ) override
        {
                if ( !spin_with_timeout( clk_, tx_done_, timeout ) )
                        return ERROR;

                // XXX: check that char is NOT present

                uint16_t count = 0;
                for ( auto s : data )
                        for ( std::byte b : s ) {
                                if ( count == tx_buffer_.size() )
                                        return ERROR;
                                tx_buffer_[count++] = b;
                        }
                if ( count == tx_buffer_.size() )
                        return ERROR;
                tx_buffer_[count++] = std::byte{ delims[0] };

                tx_done_ = false;

                if ( HAL_UART_Transmit_DMA(
                         uart_, reinterpret_cast< uint8_t* >( tx_buffer_.data() ), count ) !=
                     HAL_OK ) {
                        return ERROR;
                }

                return SUCCESS;
        }

private:
        sntr::sentry sentry_;
        clk_iface&   clk_;

        UART_HandleTypeDef* uart_   = nullptr;
        DMA_HandleTypeDef*  tx_dma_ = nullptr;

        bool volatile tx_done_ = true;
        std::array< std::byte, 1024 > tx_buffer_;

        std::byte rx_byte_;
        // XXX: the char might be templated
        bits::char_rx_container< Delims... > rx_;
};

}  // namespace servio::drv
