#include "base/drv_interfaces.hpp"
#include "base/sentry.hpp"
#include "base/status.hpp"
#include "platform.hpp"

#include <emlabcpp/experimental/cobs.h>
#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/result.h>
#include <emlabcpp/static_circular_buffer.h>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

class cobs_uart : public base::com_interface, public base::observed_interface
{
public:
        cobs_uart(
            base::central_sentry& central,
            UART_HandleTypeDef&   uart,
            DMA_HandleTypeDef&    tx_dma );

        cobs_uart( const cobs_uart& )            = delete;
        cobs_uart( cobs_uart&& )                 = delete;
        cobs_uart& operator=( const cobs_uart& ) = delete;
        cobs_uart& operator=( cobs_uart&& )      = delete;

        void rx_cplt_irq( UART_HandleTypeDef* huart );
        void tx_cplt_irq( UART_HandleTypeDef* huart );
        void err_irq( UART_HandleTypeDef* huart );

        base::status get_status() const override;
        void         clear_status() override;

        base::com_res load_message( em::view< std::byte* > data ) override;

        em::result start() override;

        em::result send( em::view< const std::byte* > data ) override;

private:
        // we tolerate overrun, dma, frame, noise error - we can recover
        static constexpr uint32_t tolerable_errors =
            HAL_UART_ERROR_ORE | HAL_UART_ERROR_DMA | HAL_UART_ERROR_FE | HAL_UART_ERROR_NE;

        base::sentry sentry_;

        UART_HandleTypeDef* uart_   = nullptr;
        DMA_HandleTypeDef*  tx_dma_ = nullptr;

        volatile bool                tx_done_ = true;
        std::array< std::byte, 128 > tx_buffer_;

        uint16_t                                     received_size_ = 0;
        em::cobs_decoder                             cobs_dec_;
        std::byte                                    rx_byte_;
        em::static_circular_buffer< std::byte, 256 > rx_buffer_;
        em::static_circular_buffer< uint16_t, 8 >    rx_sizes_;

        uint32_t error_status_ = 0;
};

inline cobs_uart::cobs_uart(
    base::central_sentry& central,
    UART_HandleTypeDef&   uart,
    DMA_HandleTypeDef&    tx_dma )
  : sentry_( central )
  , uart_( &uart )
  , tx_dma_( &tx_dma )
{
}

inline void cobs_uart::rx_cplt_irq( UART_HandleTypeDef* huart )
{
        if ( huart != uart_ )
                return;

        if ( rx_byte_ == std::byte{ 0 } ) {
                rx_sizes_.push_back( received_size_ - 1 );
                received_size_ = 0;
        } else if ( received_size_ == 0 ) {
                cobs_dec_ = em::cobs_decoder{ rx_byte_ };
                received_size_ += 1;
        } else {
                const std::optional< std::byte > b = cobs_dec_.iter( rx_byte_ );
                if ( b.has_value() ) {
                        received_size_ += 1;
                        rx_buffer_.push_back( *b );
                }
        }
        if ( start() != em::SUCCESS )
                sentry_.set_inoperable();
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

        error_status_ = HAL_UART_GetError( huart );
        if ( ( error_status_ & ~tolerable_errors ) != 0 )
                sentry_.set_inoperable();
}

inline em::result cobs_uart::start()
{
        if ( HAL_UART_Receive_IT( uart_, reinterpret_cast< uint8_t* >( &rx_byte_ ), 1 ) != HAL_OK )
                return em::ERROR;
        return em::SUCCESS;
}

}  // namespace servio::drv
