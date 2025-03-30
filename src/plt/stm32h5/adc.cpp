#include "../../fw/util.hpp"
#include "../platform.hpp"
#include "setup.hpp"

namespace servio::plt
{

void setup_adc_channel( ADC_ChannelConfTypeDef& channel, uint32_t ch, opt< drv::pin_cfg > cfg )
{
        channel = ADC_ChannelConfTypeDef{
            .Channel      = ch,
            .Rank         = ADC_REGULAR_RANK_1,
            .SamplingTime = ADC_SAMPLETIME_47CYCLES_5,
            .SingleDiff   = ADC_SINGLE_ENDED,
            .OffsetNumber = ADC_OFFSET_NONE,
            .Offset       = 0,
            .OffsetSign   = 0,
        };

        if ( cfg.has_value() )
                setup_gpio( *cfg );
}

status setup_adc( ADC_HandleTypeDef& adc, DMA_HandleTypeDef& dma, adc_cfg cfg )
{
        adc.Instance                  = cfg.adc_instance;
        adc.Init.ClockPrescaler       = ADC_CLOCK_ASYNC_DIV1;
        adc.Init.Resolution           = ADC_RESOLUTION_12B;
        adc.Init.DataAlign            = ADC_DATAALIGN_RIGHT;
        adc.Init.ScanConvMode         = ADC_SCAN_DISABLE;
        adc.Init.EOCSelection         = ADC_EOC_SINGLE_CONV;
        adc.Init.NbrOfConversion      = 1;
        adc.Init.ExternalTrigConv     = ADC_EXTERNALTRIG_T6_TRGO;
        adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
        adc.Init.Overrun              = ADC_OVR_DATA_PRESERVED;

        dma.Instance                   = cfg.dma.instance;
        dma.Init.Request               = cfg.dma.request;
        dma.Init.Direction             = DMA_PERIPH_TO_MEMORY;
        dma.Init.SrcInc                = DMA_SINC_FIXED;
        dma.Init.DestInc               = DMA_DINC_INCREMENTED;
        dma.Init.SrcDataWidth          = DMA_SRC_DATAWIDTH_HALFWORD;
        dma.Init.DestDataWidth         = DMA_DEST_DATAWIDTH_HALFWORD;
        dma.Init.SrcBurstLength        = 1;
        dma.Init.DestBurstLength       = 1;
        dma.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        dma.Init.TransferEventMode     = DMA_TCEM_BLOCK_TRANSFER;
        dma.Init.Mode                  = DMA_NORMAL;
        dma.Init.Priority              = cfg.dma.priority;

        if ( HAL_ADC_Init( &adc ) != HAL_OK )
                return ERROR;

        if ( HAL_DMA_Init( &dma ) != HAL_OK )
                return ERROR;

        __HAL_LINKDMA( ( &adc ), DMA_Handle, dma );

        if ( HAL_DMA_ConfigChannelAttributes( &dma, DMA_CHANNEL_NPRIV ) != HAL_OK )
                return ERROR;

        HAL_NVIC_SetPriority( ADC1_IRQn, cfg.adc_irq_priority, 0 );
        HAL_NVIC_EnableIRQ( ADC1_IRQn );

        HAL_NVIC_SetPriority( cfg.dma.irq, cfg.dma.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.dma.irq );

        return SUCCESS;
}

}  // namespace servio::plt
