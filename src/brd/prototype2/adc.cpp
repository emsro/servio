#include "fw/board.hpp"
#include "fw/util.hpp"
#include "setup.hpp"

#include <cstdlib>

namespace servio::brd
{

void setup_adc_channel( ADC_ChannelConfTypeDef& channel, drv::pinch_cfg cfg )
{
        channel = ADC_ChannelConfTypeDef{
            .Channel      = cfg.channel,
            .Rank         = ADC_REGULAR_RANK_1,
            .SamplingTime = ADC_SAMPLETIME_92CYCLES_5,
            .SingleDiff   = ADC_SINGLE_ENDED,
            .OffsetNumber = ADC_OFFSET_NONE,
            .Offset       = 0,
            .OffsetSign   = 0,
        };

        if ( cfg.port == nullptr ) {
                return;
        }
        GPIO_InitTypeDef gpio_c{};
        gpio_c.Pin  = cfg.pin;
        gpio_c.Mode = GPIO_MODE_ANALOG;
        gpio_c.Pull = GPIO_NOPULL;
        HAL_GPIO_Init( cfg.port, &gpio_c );
}

em::result setup_adc( ADC_HandleTypeDef& adc, DMA_HandleTypeDef& dma, adc_cfg cfg )
{

        adc.Instance                  = cfg.adc_instance;
        adc.Init.ClockPrescaler       = ADC_CLOCK_SYNC_PCLK_DIV2;
        adc.Init.Resolution           = ADC_RESOLUTION_12B;
        adc.Init.DataAlign            = ADC_DATAALIGN_RIGHT;
        adc.Init.ScanConvMode         = ADC_SCAN_DISABLE;
        adc.Init.EOCSelection         = ADC_EOC_SINGLE_CONV;
        adc.Init.NbrOfConversion      = 1;
        adc.Init.ExternalTrigConv     = ADC_EXTERNALTRIG_T4_TRGO;
        adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
        adc.Init.Overrun              = ADC_OVR_DATA_PRESERVED;

        dma.Instance                 = cfg.dma.instance;
        dma.Init.Request             = cfg.dma.request;
        dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        dma.Init.PeriphInc           = DMA_PINC_DISABLE;
        dma.Init.MemInc              = DMA_MINC_ENABLE;
        dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        dma.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        dma.Init.Mode                = DMA_NORMAL;
        dma.Init.Priority            = cfg.dma.priority;

        if ( HAL_ADC_Init( &adc ) != HAL_OK ) {
                fw::stop_exec();
        }

        ADC_MultiModeTypeDef mmode{};
        mmode.Mode = ADC_MODE_INDEPENDENT;

        if ( HAL_ADCEx_MultiModeConfigChannel( &adc, &mmode ) != HAL_OK ) {
                fw::stop_exec();
        }

        HAL_NVIC_SetPriority( cfg.dma.irq, cfg.dma.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.dma.irq );

        if ( HAL_DMA_Init( &dma ) != HAL_OK ) {
                fw::stop_exec();
        }

        __HAL_LINKDMA( ( &adc ), DMA_Handle, dma );

        HAL_NVIC_SetPriority( ADC1_2_IRQn, cfg.adc_irq_priority, 0 );
        HAL_NVIC_EnableIRQ( ADC1_2_IRQn );

        return em::SUCCESS;
}

}  // namespace servio::brd
