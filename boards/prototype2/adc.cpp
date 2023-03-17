#include "fw/board.hpp"
#include "fw/util.hpp"

#include <cstdlib>

namespace brd
{

/// external opamp:
///     - 0...3v3
///     - 100 GAIN
///     - connected to PA3
///     - amplifies current sense resistor from Hbridge
///     - resisotr is 0.043 Ohm
///
/// mapping:
///   pot_sense - PA0 - IN1
///   vbat_sense - PA1 - IN2
///   circ_current - PA2 - IN3
///   low_side_current - PA3 - IN4
///
bool setup_adc( fw::acquisition::handles& h )
{

        h.adc.Instance                   = ADC1;
        h.adc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
        h.adc.Init.Resolution            = ADC_RESOLUTION_12B;
        h.adc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
        h.adc.Init.GainCompensation      = 0;
        h.adc.Init.ScanConvMode          = ADC_SCAN_DISABLE;
        h.adc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
        h.adc.Init.LowPowerAutoWait      = DISABLE;
        h.adc.Init.ContinuousConvMode    = DISABLE;
        h.adc.Init.NbrOfConversion       = 1;
        h.adc.Init.DiscontinuousConvMode = DISABLE;
        h.adc.Init.ExternalTrigConv      = ADC_EXTERNALTRIG_T4_TRGO;
        h.adc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
        h.adc.Init.DMAContinuousRequests = DISABLE;
        h.adc.Init.Overrun               = ADC_OVR_DATA_PRESERVED;
        h.adc.Init.OversamplingMode      = DISABLE;

        h.adc_irqn = ADC1_2_IRQn;

        ADC_MultiModeTypeDef mmode{};

        mmode.Mode = ADC_MODE_INDEPENDENT;

        h.current_chconf.Channel  = ADC_CHANNEL_4;
        h.position_chconf.Channel = ADC_CHANNEL_1;
        h.vcc_chconf.Channel      = ADC_CHANNEL_2;
        h.temp_chconf.Channel     = ADC_CHANNEL_TEMPSENSOR_ADC1;
        for ( ADC_ChannelConfTypeDef* chconf :
              { &h.current_chconf, &h.position_chconf, &h.vcc_chconf, &h.temp_chconf } ) {
                chconf->Rank         = ADC_REGULAR_RANK_1;
                chconf->SamplingTime = ADC_SAMPLETIME_92CYCLES_5;
                chconf->SingleDiff   = ADC_SINGLE_ENDED;
                chconf->OffsetNumber = ADC_OFFSET_NONE;
                chconf->Offset       = 0;
        }

        h.dma.Instance                 = DMA1_Channel1;
        h.dma.Init.Request             = DMA_REQUEST_ADC1;
        h.dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        h.dma.Init.PeriphInc           = DMA_PINC_DISABLE;
        h.dma.Init.MemInc              = DMA_MINC_ENABLE;
        h.dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        h.dma.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        h.dma.Init.Mode                = DMA_NORMAL;
        h.dma.Init.Priority            = DMA_PRIORITY_LOW;  // TODO: check different priorty

        GPIO_InitTypeDef gpio_c{};
        gpio_c.Pin  = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
        gpio_c.Mode = GPIO_MODE_ANALOG;
        gpio_c.Pull = GPIO_NOPULL;

        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        HAL_GPIO_Init( GPIOA, &gpio_c );

        if ( HAL_ADC_Init( &h.adc ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_ADCEx_MultiModeConfigChannel( &h.adc, &mmode ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_ADC_ConfigChannel( &h.adc, &h.current_chconf ) != HAL_OK ) {
                fw::stop_exec();
        }

        HAL_NVIC_SetPriority( DMA1_Channel1_IRQn, 0, 0 );
        HAL_NVIC_EnableIRQ( DMA1_Channel1_IRQn );

        if ( HAL_DMA_Init( &h.dma ) != HAL_OK ) {
                fw::stop_exec();
        }

        __HAL_LINKDMA( ( &h.adc ), DMA_Handle, h.dma );

        HAL_NVIC_SetPriority( h.adc_irqn, 0, 0 );
        HAL_NVIC_EnableIRQ( h.adc_irqn );

        return true;
}

}  // namespace brd
