#include "fw/util.hpp"
#include "setup.hpp"

#include <cstdlib>

namespace plt
{

// RX to PA10
// TX to PB6

em::result setup_uart( UART_HandleTypeDef& uart, DMA_HandleTypeDef& tx_dma, uart_cfg cfg )
{
        uart.Instance                    = cfg.uart_instance;
        uart.Init.BaudRate               = cfg.baudrate;
        uart.Init.WordLength             = UART_WORDLENGTH_8B;
        uart.Init.StopBits               = UART_STOPBITS_1;
        uart.Init.Parity                 = UART_PARITY_NONE;
        uart.Init.Mode                   = UART_MODE_TX_RX;
        uart.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
        uart.Init.OverSampling           = UART_OVERSAMPLING_16;
        uart.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
        uart.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
        uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

        GPIO_InitTypeDef rx_init;

        rx_init.Pin       = cfg.rx.pin;
        rx_init.Mode      = GPIO_MODE_AF_PP;
        rx_init.Pull      = GPIO_NOPULL;
        rx_init.Speed     = GPIO_SPEED_FREQ_LOW;
        rx_init.Alternate = cfg.rx.alternate;

        GPIO_InitTypeDef tx_init;

        tx_init.Pin       = cfg.tx.pin;
        tx_init.Mode      = GPIO_MODE_AF_PP;
        tx_init.Pull      = GPIO_NOPULL;
        tx_init.Speed     = GPIO_SPEED_FREQ_LOW;
        tx_init.Alternate = cfg.tx.alternate;

        tx_dma.Instance       = cfg.tx_dma.instance;
        tx_dma.Init.Request   = cfg.tx_dma.request;
        tx_dma.Init.Direction = DMA_MEMORY_TO_PERIPH;
        tx_dma.Init.Mode      = DMA_NORMAL;
        tx_dma.Init.Priority  = cfg.tx_dma.priority;

        HAL_GPIO_Init( cfg.rx.port, &rx_init );
        HAL_GPIO_Init( cfg.tx.port, &tx_init );

        HAL_NVIC_SetPriority( cfg.irq, cfg.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.irq );

        if ( HAL_DMA_Init( &tx_dma ) != HAL_OK ) {
                fw::stop_exec();
        }

        __HAL_LINKDMA( ( &uart ), hdmatx, tx_dma );

        if ( HAL_UART_Init( &uart ) != HAL_OK ) {
                //    if (HAL_RS485Ex_Init(&res.uart, UART_DE_POLARITY_HIGH, 0, 0) !=
                //    HAL_OK) {
                fw::stop_exec();
        }

        if ( HAL_UARTEx_SetTxFifoThreshold( &uart, UART_TXFIFO_THRESHOLD_1_2 ) != HAL_OK ) {
                fw::stop_exec();
        }
        if ( HAL_UARTEx_SetRxFifoThreshold( &uart, UART_RXFIFO_THRESHOLD_1_2 ) != HAL_OK ) {
                fw::stop_exec();
        }
        if ( HAL_UARTEx_EnableFifoMode( &uart ) != HAL_OK ) {
                fw::stop_exec();
        }

        return em::SUCCESS;
}

}  // namespace plt
