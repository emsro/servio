#include "protocol.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/static_function.h>
#include <stm32g4xx_hal.h>

#pragma once

namespace fw
{

class comms
{
public:
    struct handles
    {
        UART_HandleTypeDef uart;
        DMA_HandleTypeDef  tx_dma;
        DMA_HandleTypeDef  rx_dma;
    };

    using msg_callback = em::static_function< void( const master_to_servo_message& ), 16 >;

    comms() = default;

    comms( const comms& )            = delete;
    comms( comms&& )                 = delete;
    comms& operator=( const comms& ) = delete;
    comms& operator=( comms&& )      = delete;

    bool setup( em::function_view< bool( handles& ) > );

    void tx_dma_irq();
    void rx_dma_irq();
    void uart_irq();
    void rx_cplt_irq( UART_HandleTypeDef* huart );

    void set_msg_callback( msg_callback );

    void start_receiving();
    void send_msg( servo_to_master_message msg );

    void transmit( servo_to_master_variant var );

private:
    handles h_;

    std::size_t received_counter_ = 0;
    std::size_t sent_counter_     = 0;

    // TODO: 128 is random, check buffer sizes everywhere
    std::array< uint8_t, 128 >               idata_buffer_;
    master_to_servo_sequencer                iseq_;
    std::array< master_to_servo_message, 2 > imsg_buffer_;
    std::size_t                              imsg_i_;

    msg_callback msg_callback_;

    servo_to_master_message omsg_;
};

}  // namespace fw
