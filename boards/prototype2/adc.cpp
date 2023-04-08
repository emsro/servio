#include "fw/board.hpp"
#include "fw/util.hpp"
#include "setup.hpp"

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
bool setup_adc( fw::acquisition::handles& h, adc_cfg cfg )
{
        if ( cfg.adc_instance != ADC1 && cfg.adc_instance != ADC2 ) {
                // this function relies a lot on the fact that adc can be either ADC1 or ADC2
                return false;
        }

        h.adc.Instance                   = cfg.adc_instance;
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

        ADC_MultiModeTypeDef mmode{};

        mmode.Mode = ADC_MODE_INDEPENDENT;

        h.current_chconf.Channel  = cfg.current.channel;
        h.position_chconf.Channel = cfg.position.channel;
        h.vcc_chconf.Channel      = cfg.vcc.channel;
        h.temp_chconf.Channel     = cfg.temp.channel;
        for ( ADC_ChannelConfTypeDef* chconf :
              { &h.current_chconf, &h.position_chconf, &h.vcc_chconf, &h.temp_chconf } ) {
                chconf->Rank         = ADC_REGULAR_RANK_1;
                chconf->SamplingTime = ADC_SAMPLETIME_92CYCLES_5;
                chconf->SingleDiff   = ADC_SINGLE_ENDED;
                chconf->OffsetNumber = ADC_OFFSET_NONE;
                chconf->Offset       = 0;
        }

        h.dma.Instance                 = cfg.dma.instance;
        h.dma.Init.Request             = cfg.dma.request;
        h.dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        h.dma.Init.PeriphInc           = DMA_PINC_DISABLE;
        h.dma.Init.MemInc              = DMA_MINC_ENABLE;
        h.dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        h.dma.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        h.dma.Init.Mode                = DMA_NORMAL;
        h.dma.Init.Priority            = cfg.dma.priority;

        for ( const pinch_cfg& ch : { cfg.current, cfg.position, cfg.vcc, cfg.temp } ) {
                if ( ch.pin == 0 || ch.port == nullptr ) {
                        continue;
                }
                GPIO_InitTypeDef gpio_c{};
                gpio_c.Pin  = ch.pin;
                gpio_c.Mode = GPIO_MODE_ANALOG;
                gpio_c.Pull = GPIO_NOPULL;
                HAL_GPIO_Init( ch.port, &gpio_c );
        }

        if ( HAL_ADC_Init( &h.adc ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_ADCEx_MultiModeConfigChannel( &h.adc, &mmode ) != HAL_OK ) {
                fw::stop_exec();
        }

        if ( HAL_ADC_ConfigChannel( &h.adc, &h.current_chconf ) != HAL_OK ) {
                fw::stop_exec();
        }

        HAL_NVIC_SetPriority( cfg.dma.irq, cfg.dma.irq_priority, 0 );
        HAL_NVIC_EnableIRQ( cfg.dma.irq );

        if ( HAL_DMA_Init( &h.dma ) != HAL_OK ) {
                fw::stop_exec();
        }

        __HAL_LINKDMA( ( &h.adc ), DMA_Handle, h.dma );

        HAL_NVIC_SetPriority( ADC1_2_IRQn, cfg.adc_irq_priority, 0 );
        HAL_NVIC_EnableIRQ( ADC1_2_IRQn );

        return true;
}

}  // namespace brd
