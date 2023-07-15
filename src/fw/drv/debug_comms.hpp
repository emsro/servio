#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_circular_buffer.h>
#include <emlabcpp/static_vector.h>
#include <span>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;

namespace fw::drv
{

class debug_comms
{
public:
        struct handles
        {
                UART_HandleTypeDef uart;
                DMA_HandleTypeDef  tx_dma;
        };

        debug_comms() = default;

        debug_comms( const debug_comms& )            = delete;
        debug_comms( debug_comms&& )                 = delete;
        debug_comms& operator=( const debug_comms& ) = delete;
        debug_comms& operator=( debug_comms&& )      = delete;

        bool setup( em::function_view< bool( handles& ) > setup_f );

        void tx_dma_irq();
        void uart_irq();
        void rx_cplt_irq( UART_HandleTypeDef* huart );

        void start();

        void transmit( std::span< const std::byte > );

        em::static_vector< std::byte, 32 > receive( std::size_t size = 32 );

private:
        handles h_;

        std::byte                                    ibyte_;
        em::static_circular_buffer< std::byte, 128 > idata_buffer_{};
        std::array< std::byte, 128 >                 odata_buffer_{};
};

}  // namespace fw::drv
