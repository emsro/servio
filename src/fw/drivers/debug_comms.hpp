
#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_circular_buffer.h>
#include <emlabcpp/static_vector.h>
#include <span>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;

namespace fw
{

class debug_comms
{
public:
    struct handles
    {
        UART_HandleTypeDef uart;
        DMA_HandleTypeDef  tx_dma;
        DMA_HandleTypeDef  rx_dma;
    };

    debug_comms() = default;

    debug_comms( const debug_comms& )            = delete;
    debug_comms( debug_comms&& )                 = delete;
    debug_comms& operator=( const debug_comms& ) = delete;
    debug_comms& operator=( debug_comms&& )      = delete;

    bool setup( em::function_view< bool( handles& ) > setup_f );

    void tx_dma_irq();
    void rx_dma_irq();
    void uart_irq();
    void rx_cplt_irq( UART_HandleTypeDef* huart );

    void start_receiving();

    void transmit( std::span< uint8_t > );

    std::optional< em::static_vector< uint8_t, 64 > > receive( std::size_t size );

private:
    handles h_;

    uint8_t                                    ibyte_;
    em::static_circular_buffer< uint8_t, 128 > idata_buffer_{};
    std::array< uint8_t, 128 >                 odata_buffer_{};
};

}  // namespace fw
