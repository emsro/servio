#include "fw/board.hpp"
#include "fw/util.hpp"
#include "setup.hpp"

#include <cstdlib>

// TODO: bigass duplication from IUART< but who is counting?

namespace brd
{

// RX to PA10
// TX to PB6

bool setup_duart( fw::debug_comms::handles& h, duart_cfg cfg )
{
        h.uart.Instance                    = cfg.uart_instance;
        h.uart.Init.BaudRate               = cfg.baudrate;
        h.uart.Init.WordLength             = UART_WORDLENGTH_8B;
        h.uart.Init.StopBits               = UART_STOPBITS_1;
        h.uart.Init.Parity                 = UART_PARITY_NONE;
        h.uart.Init.Mode                   = UART_MODE_TX_RX;
        h.uart.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
        h.uart.Init.OverSampling           = UART_OVERSAMPLING_16;
        h.uart.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
        h.uart.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
        h.uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

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

        h.tx_dma.Instance                 = cfg.tx_dma.instance;
        h.tx_dma.Init.Request             = cfg.tx_dma.request;
        h.tx_dma.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        h.tx_dma.Init.PeriphInc           = DMA_PINC_DISABLE;
        h.tx_dma.Init.MemInc              = DMA_MINC_ENABLE;
        h.tx_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        h.tx_dma.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        h.tx_dma.Init.Mode                = DMA_NORMAL;
        h.tx_dma.Init.Priority            = cfg.tx_dma.priority;

        h.rx_dma.Instance                 = cfg.rx_dma.instance;
        h.rx_dma.Init.Request             = cfg.rx_dma.request;
        h.rx_dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        h.rx_dma.Init.PeriphInc           = DMA_PINC_DISABLE;
        h.rx_dma.Init.MemInc              = DMA_MINC_ENABLE;
        h.rx_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        h.rx_dma.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        h.rx_dma.Init.Mode                = DMA_NORMAL;
        h.rx_dma.Init.Priority            = cfg.rx_dma.priority;

        HAL_GPIO_Init( cfg.rx.port, &rx_init );
        HAL_GPIO_Init( cfg.tx.port, &tx_init );

        HAL_NVIC_SetPriority( cfg.tx_dma.irq, cfg.tx_dma.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.tx_dma.irq );

        HAL_NVIC_SetPriority( cfg.rx_dma.irq, cfg.rx_dma.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.rx_dma.irq );

        HAL_NVIC_SetPriority( cfg.irq, cfg.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.irq );

        if ( HAL_DMA_Init( &h.tx_dma ) != HAL_OK ) {
                fw::stop_exec();
        }

        __HAL_LINKDMA( ( &h.uart ), hdmatx, h.tx_dma );

        if ( HAL_DMA_Init( &h.rx_dma ) != HAL_OK ) {
                fw::stop_exec();
        }

        __HAL_LINKDMA( ( &h.uart ), hdmarx, h.rx_dma );

        if ( HAL_UART_Init( &h.uart ) != HAL_OK ) {
                //    if (HAL_RS485Ex_Init(&res.uart, UART_DE_POLARITY_HIGH, 0, 0) !=
                //    HAL_OK) {
                fw::stop_exec();
        }

        if ( HAL_UARTEx_SetTxFifoThreshold( &h.uart, UART_TXFIFO_THRESHOLD_1_2 ) != HAL_OK ) {
                fw::stop_exec();
        }
        if ( HAL_UARTEx_SetRxFifoThreshold( &h.uart, UART_RXFIFO_THRESHOLD_1_2 ) != HAL_OK ) {
                fw::stop_exec();
        }
        if ( HAL_UARTEx_EnableFifoMode( &h.uart ) != HAL_OK ) {
                fw::stop_exec();
        }

        return true;
}

}  // namespace brd
