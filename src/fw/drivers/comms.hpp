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
    };

    using msg_callback = em::static_function< void( const master_to_servo_message& ), 16 >;

    comms() = default;

    comms( const comms& )            = delete;
    comms( comms&& )                 = delete;
    comms& operator=( const comms& ) = delete;
    comms& operator=( comms&& )      = delete;

    bool setup( em::function_view< bool( handles& ) > );

    void tx_dma_irq();
    void uart_irq();
    void rx_cplt_irq( UART_HandleTypeDef* huart );

    std::variant< std::monostate, master_to_servo_variant, em::protocol::error_record > get_message();

    void start();

    void send( const servo_to_master_variant& var );

private:
    handles h_;

    uint8_t rx_byte_;

    servo_endpoint ep_;

    servo_to_master_message omsg_;
};

}  // namespace fw
