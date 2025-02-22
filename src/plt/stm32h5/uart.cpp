#include "../../fw/util.hpp"
#include "setup.hpp"

#include <cstdlib>

namespace servio::plt
{

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

        setup_gpio( cfg.rx );
        setup_gpio( cfg.tx );

        tx_dma.Instance                   = cfg.tx_dma.instance;
        tx_dma.Init.BlkHWRequest          = DMA_BREQ_SINGLE_BURST;
        tx_dma.Init.DestBurstLength       = 1;
        tx_dma.Init.DestDataWidth         = DMA_DEST_DATAWIDTH_BYTE;
        tx_dma.Init.DestInc               = DMA_DINC_FIXED;
        tx_dma.Init.Direction             = DMA_MEMORY_TO_PERIPH;
        tx_dma.Init.Mode                  = DMA_NORMAL;
        tx_dma.Init.Priority              = cfg.tx_dma.priority;
        tx_dma.Init.Request               = cfg.tx_dma.request;
        tx_dma.Init.SrcBurstLength        = 1;
        tx_dma.Init.SrcDataWidth          = DMA_SRC_DATAWIDTH_BYTE;
        tx_dma.Init.SrcInc                = DMA_SINC_INCREMENTED;
        tx_dma.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        tx_dma.Init.TransferEventMode     = DMA_TCEM_BLOCK_TRANSFER;

        if ( HAL_DMA_Init( &tx_dma ) != HAL_OK )
                fw::stop_exec();

        __HAL_LINKDMA( ( &uart ), hdmatx, tx_dma );

        if ( HAL_DMA_ConfigChannelAttributes( &tx_dma, DMA_CHANNEL_NPRIV ) != HAL_OK )
                fw::stop_exec();

        HAL_NVIC_SetPriority( cfg.tx_dma.irq, cfg.tx_dma.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.tx_dma.irq );

        if ( HAL_UART_Init( &uart ) != HAL_OK )
                fw::stop_exec();

        if ( HAL_UARTEx_SetTxFifoThreshold( &uart, UART_TXFIFO_THRESHOLD_1_2 ) != HAL_OK )
                fw::stop_exec();
        if ( HAL_UARTEx_SetRxFifoThreshold( &uart, UART_RXFIFO_THRESHOLD_1_2 ) != HAL_OK )
                fw::stop_exec();
        if ( HAL_UARTEx_EnableFifoMode( &uart ) != HAL_OK )
                fw::stop_exec();

        HAL_NVIC_SetPriority( cfg.irq, cfg.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.irq );
        return em::SUCCESS;
}

}  // namespace servio::plt
