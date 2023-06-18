#include "protocol.hpp"

#include <emlabcpp/experimental/cobs.h>
#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_circular_buffer.h>
#include <emlabcpp/static_function.h>
#include <stm32g4xx_hal.h>

#pragma once

namespace fw
{

constexpr std::size_t comm_buff_size = 128;

class comms
{
public:
        struct handles
        {
                UART_HandleTypeDef uart;
                DMA_HandleTypeDef  tx_dma;
        };

        comms() = default;

        comms( const comms& )            = delete;
        comms( comms&& )                 = delete;
        comms& operator=( const comms& ) = delete;
        comms& operator=( comms&& )      = delete;

        bool setup( em::function_view< bool( handles& ) > );

        void tx_dma_irq();
        void uart_irq();
        void rx_cplt_irq( UART_HandleTypeDef* huart );

        std::tuple< bool, em::view< std::byte* > > load_message( em::view< std::byte* > data );

        void start();

        void send( em::view< std::byte* > data );

private:
        handles h_;

        uint16_t                                     current_size_ = 0;
        em::cobs_decoder                             cd_;
        std::byte                                    rx_byte_;
        em::static_circular_buffer< std::byte, 256 > ibuffer_;
        em::static_circular_buffer< uint16_t, 8 >    isizes_;

        std::array< std::byte, comm_buff_size > otmp_;
};

}  // namespace fw
