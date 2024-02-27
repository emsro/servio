#include "base/drv_interfaces.hpp"
#include "platform.hpp"

#include <emlabcpp/experimental/cobs.h>
#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_circular_buffer.h>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

class cobs_uart : public base::com_interface
{
public:
        cobs_uart() = default;

        cobs_uart( const cobs_uart& )            = delete;
        cobs_uart( cobs_uart&& )                 = delete;
        cobs_uart& operator=( const cobs_uart& ) = delete;
        cobs_uart& operator=( cobs_uart&& )      = delete;

        cobs_uart* setup( UART_HandleTypeDef& uart, DMA_HandleTypeDef& tx_dma );

        void rx_cplt_irq( UART_HandleTypeDef* huart );
        void tx_cplt_irq( UART_HandleTypeDef* huart );

        base::com_res load_message( em::view< std::byte* > data ) override;

        em::result start() override;

        em::result send( em::view< const std::byte* > data ) override;

private:
        UART_HandleTypeDef* uart_   = {};
        DMA_HandleTypeDef*  tx_dma_ = {};

        volatile bool                                tx_done_      = true;
        uint16_t                                     current_size_ = 0;
        em::cobs_decoder                             cd_;
        std::byte                                    rx_byte_;
        em::static_circular_buffer< std::byte, 256 > ibuffer_;
        em::static_circular_buffer< uint16_t, 8 >    isizes_;

        std::array< std::byte, 128 > otmp_;
};

inline cobs_uart* cobs_uart::setup( UART_HandleTypeDef& uart, DMA_HandleTypeDef& tx_dma )
{
        uart_   = &uart;
        tx_dma_ = &tx_dma;
        return this;
}

inline void cobs_uart::rx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != uart_ )
                return;

        if ( rx_byte_ == std::byte{ 0 } ) {
                isizes_.push_back( current_size_ - 1 );
                current_size_ = 0;
        } else if ( current_size_ == 0 ) {
                cd_ = em::cobs_decoder{ rx_byte_ };
                current_size_ += 1;
        } else {
                const std::optional< std::byte > b = cd_.iter( rx_byte_ );
                if ( b.has_value() ) {
                        current_size_ += 1;
                        ibuffer_.push_back( *b );
                }
        }
        std::ignore = start();
}

inline void cobs_uart::tx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != uart_ )
                return;
        tx_done_ = true;
}

inline em::result cobs_uart::start()
{
        if ( HAL_UART_Receive_IT( uart_, reinterpret_cast< uint8_t* >( &rx_byte_ ), 1 ) != HAL_OK )
                return em::ERROR;
        return em::SUCCESS;
}

}  // namespace servio::drv
