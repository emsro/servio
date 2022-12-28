#include "fw/board.hpp"
#include "fw/util.hpp"

#include <cstdlib>

// TODO: enable clock in clock setup?

namespace fw
{

bool setup_iuart( comms::handles& res )
{
    res.uart.Instance                    = USART2;
    res.uart.Init.BaudRate               = 115200;
    res.uart.Init.WordLength             = UART_WORDLENGTH_8B;
    res.uart.Init.StopBits               = UART_STOPBITS_1;
    res.uart.Init.Parity                 = UART_PARITY_NONE;
    res.uart.Init.Mode                   = UART_MODE_TX_RX;
    res.uart.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    res.uart.Init.OverSampling           = UART_OVERSAMPLING_16;
    res.uart.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    res.uart.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
    res.uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    GPIO_InitTypeDef ginitb;

    ginitb.Pin       = GPIO_PIN_3 | GPIO_PIN_4;
    ginitb.Mode      = GPIO_MODE_AF_PP;
    ginitb.Pull      = GPIO_NOPULL;
    ginitb.Speed     = GPIO_SPEED_FREQ_LOW;
    ginitb.Alternate = GPIO_AF7_USART2;

    res.tx_dma.Instance                 = DMA1_Channel4;
    res.tx_dma.Init.Request             = DMA_REQUEST_USART2_TX;
    res.tx_dma.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    res.tx_dma.Init.PeriphInc           = DMA_PINC_DISABLE;
    res.tx_dma.Init.MemInc              = DMA_MINC_ENABLE;
    res.tx_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    res.tx_dma.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    res.tx_dma.Init.Mode                = DMA_NORMAL;
    res.tx_dma.Init.Priority            = DMA_PRIORITY_LOW;

    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_Init( GPIOB, &ginitb );

    if ( HAL_UART_Init( &res.uart ) != HAL_OK ) {
        stop_exec();
    }

    HAL_NVIC_SetPriority( DMA1_Channel3_IRQn, 1, 0 );
    HAL_NVIC_EnableIRQ( DMA1_Channel3_IRQn );

    HAL_NVIC_SetPriority( DMA1_Channel4_IRQn, 1, 0 );
    HAL_NVIC_EnableIRQ( DMA1_Channel4_IRQn );

    HAL_NVIC_SetPriority( USART2_IRQn, 0, 0 );
    HAL_NVIC_EnableIRQ( USART2_IRQn );

    if ( HAL_DMA_Init( &res.tx_dma ) != HAL_OK ) {
        stop_exec();
    }

    __HAL_LINKDMA( ( &res.uart ), hdmatx, res.tx_dma );

    if ( HAL_UARTEx_SetTxFifoThreshold( &res.uart, UART_TXFIFO_THRESHOLD_1_8 ) != HAL_OK ) {
        stop_exec();
    }
    if ( HAL_UARTEx_SetRxFifoThreshold( &res.uart, UART_RXFIFO_THRESHOLD_1_8 ) != HAL_OK ) {
        stop_exec();
    }
    if ( HAL_UARTEx_DisableFifoMode( &res.uart ) != HAL_OK ) {
        stop_exec();
    }

    return true;
}

}  // namespace fw
