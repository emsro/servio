#include "fw/board.hpp"
#include "fw/util.hpp"

#include <cstdlib>

// TODO: bigass duplication from IUART< but who is counting?

namespace brd
{

// RX to PA10
// TX to PB6

bool setup_duart( fw::debug_comms::handles& h )
{
        h.uart.Instance                    = USART1;
        h.uart.Init.BaudRate               = 460800;
        h.uart.Init.WordLength             = UART_WORDLENGTH_8B;
        h.uart.Init.StopBits               = UART_STOPBITS_1;
        h.uart.Init.Parity                 = UART_PARITY_NONE;
        h.uart.Init.Mode                   = UART_MODE_TX_RX;
        h.uart.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
        h.uart.Init.OverSampling           = UART_OVERSAMPLING_16;
        h.uart.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
        h.uart.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
        h.uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

        GPIO_InitTypeDef ginita;

        ginita.Pin       = GPIO_PIN_10;
        ginita.Mode      = GPIO_MODE_AF_PP;
        ginita.Pull      = GPIO_NOPULL;
        ginita.Speed     = GPIO_SPEED_FREQ_LOW;
        ginita.Alternate = GPIO_AF7_USART1;

        GPIO_InitTypeDef ginitb;

        ginitb.Pin       = GPIO_PIN_6;
        ginitb.Mode      = GPIO_MODE_AF_PP;
        ginitb.Pull      = GPIO_NOPULL;
        ginitb.Speed     = GPIO_SPEED_FREQ_LOW;
        ginitb.Alternate = GPIO_AF7_USART1;

        h.tx_dma.Instance                 = DMA1_Channel5;
        h.tx_dma.Init.Request             = DMA_REQUEST_USART1_TX;
        h.tx_dma.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        h.tx_dma.Init.PeriphInc           = DMA_PINC_DISABLE;
        h.tx_dma.Init.MemInc              = DMA_MINC_ENABLE;
        h.tx_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        h.tx_dma.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        h.tx_dma.Init.Mode                = DMA_NORMAL;
        h.tx_dma.Init.Priority            = DMA_PRIORITY_LOW;

        h.rx_dma.Instance                 = DMA1_Channel2;
        h.rx_dma.Init.Request             = DMA_REQUEST_USART1_RX;
        h.rx_dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        h.rx_dma.Init.PeriphInc           = DMA_PINC_DISABLE;
        h.rx_dma.Init.MemInc              = DMA_MINC_ENABLE;
        h.rx_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        h.rx_dma.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        h.rx_dma.Init.Mode                = DMA_NORMAL;
        h.rx_dma.Init.Priority            = DMA_PRIORITY_LOW;

        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        HAL_GPIO_Init( GPIOA, &ginita );
        HAL_GPIO_Init( GPIOB, &ginitb );

        HAL_NVIC_SetPriority( DMA1_Channel5_IRQn, 1, 0 );
        HAL_NVIC_EnableIRQ( DMA1_Channel5_IRQn );

        HAL_NVIC_SetPriority( DMA1_Channel2_IRQn, 1, 0 );
        HAL_NVIC_EnableIRQ( DMA1_Channel2_IRQn );

        HAL_NVIC_SetPriority( USART1_IRQn, 1, 0 );
        HAL_NVIC_EnableIRQ( USART1_IRQn );

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
