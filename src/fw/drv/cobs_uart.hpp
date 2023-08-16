#include "drv_interfaces.hpp"

#include <emlabcpp/experimental/cobs.h>
#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_circular_buffer.h>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;

namespace fw::drv
{

constexpr std::size_t comm_buff_size = 128;

class cobs_uart : public com_interface
{
public:
        struct handles
        {
                UART_HandleTypeDef uart;
                DMA_HandleTypeDef  tx_dma;
        };

        cobs_uart() = default;

        cobs_uart( const cobs_uart& )            = delete;
        cobs_uart( cobs_uart&& )                 = delete;
        cobs_uart& operator=( const cobs_uart& ) = delete;
        cobs_uart& operator=( cobs_uart&& )      = delete;

        bool setup( em::function_view< bool( handles& ) > );

        void tx_dma_irq();
        void uart_irq();
        void rx_cplt_irq( UART_HandleTypeDef* huart );
        void tx_cplt_irq( UART_HandleTypeDef* huart );

        com_res load_message( em::view< std::byte* > data );

        void start();

        bool send( em::view< const std::byte* > data );

private:
        handles h_;

        volatile bool                                tx_done_      = true;
        uint16_t                                     current_size_ = 0;
        em::cobs_decoder                             cd_;
        std::byte                                    rx_byte_;
        em::static_circular_buffer< std::byte, 256 > ibuffer_;
        em::static_circular_buffer< uint16_t, 8 >    isizes_;

        std::array< std::byte, comm_buff_size > otmp_;
};

}  // namespace fw::drv
