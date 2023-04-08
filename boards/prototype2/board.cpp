
#include "fw/board.hpp"

#include "setup.hpp"

namespace brd
{

fw::clock       CLOCK{};
fw::acquisition ACQUISITION{};
fw::comms       COMMS{};
fw::debug_comms DEBUG_COMMS{};
fw::hbridge     HBRIDGE{};
fw::leds        LEDS;

}  // namespace brd

extern "C" {

void TIM1_UP_TIM16_IRQHandler()
{
        brd::HBRIDGE.timer_irq();
}

void USART2_IRQHandler( void )
{
        brd::COMMS.uart_irq();
}

void DMA1_Channel4_IRQHandler( void )
{
        brd::COMMS.tx_dma_irq();
}

void USART1_IRQHandler( void )
{
        brd::DEBUG_COMMS.uart_irq();
}

void DMA1_Channel2_IRQHandler( void )
{
        brd::DEBUG_COMMS.rx_dma_irq();
}

void DMA1_Channel5_IRQHandler( void )
{
        brd::DEBUG_COMMS.tx_dma_irq();
}

void DMA1_Channel1_IRQHandler()
{
        brd::ACQUISITION.dma_irq();
}
void ADC1_2_IRQHandler()
{
        brd::ACQUISITION.adc_irq();
}

void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* h )
{
        brd::ACQUISITION.adc_conv_cplt_irq( h );
}
void HAL_ADC_ErrorCallback( ADC_HandleTypeDef* h )
{
        brd::ACQUISITION.adc_error_irq( h );
}
}

namespace brd
{

void setup_board()
{
        fw::hal_check{} << HAL_Init();
        setup_clk();
}

fw::clock* setup_clock()
{
        if ( !CLOCK.setup( setup_clock_timer ) ) {
                return nullptr;
        }
        return &CLOCK;
}

fw::acquisition* setup_acquisition()
{
        auto acq_setup = []( fw::acquisition::handles& h ) {
                __HAL_RCC_ADC12_CLK_ENABLE();
                __HAL_RCC_DMA1_CLK_ENABLE();
                __HAL_RCC_DMAMUX1_CLK_ENABLE();
                __HAL_RCC_GPIOA_CLK_ENABLE();
                __HAL_RCC_TIM4_CLK_ENABLE();
                return setup_adc(
                           h,
                           adc_cfg{
                               .adc_instance     = ADC1,
                               .adc_irq_priority = 0,
                               .dma =
                                   dma_cfg{
                                       .instance     = DMA1_Channel1,
                                       .irq          = DMA1_Channel1_IRQn,
                                       .irq_priority = 0,
                                       .request      = DMA_REQUEST_ADC1,
                                   },
                               .current =
                                   adc_pch{
                                       .channel = ADC_CHANNEL_4,
                                       .pin     = GPIO_PIN_3,
                                       .port    = GPIOA,
                                   },
                               .position =
                                   adc_pch{
                                       .channel = ADC_CHANNEL_1,
                                       .pin     = GPIO_PIN_0,
                                       .port    = GPIOA,
                                   },
                               .vcc =
                                   adc_pch{
                                       .channel = ADC_CHANNEL_2,
                                       .pin     = GPIO_PIN_1,
                                       .port    = GPIOA,
                                   },
                               .temp =
                                   adc_pch{
                                       .channel = ADC_CHANNEL_TEMPSENSOR_ADC1,
                                       .pin     = 0,
                                       .port    = nullptr,
                                   },
                           } ) &&
                       setup_adc_timer(
                           h,
                           adc_timer_cfg{
                               .timer_instance = TIM4,
                               .channel        = TIM_CHANNEL_1,
                           } );
        };
        if ( !ACQUISITION.setup( acq_setup ) ) {
                return nullptr;
        }
        return &ACQUISITION;
}

fw::hbridge* setup_hbridge()
{
        auto setup_f = []( fw::hbridge::handles& h ) {
                bool res = setup_hbridge_timers( h );

                fw::hal_check{} << HAL_TIM_RegisterCallback(
                    &h.timer, HAL_TIM_PERIOD_ELAPSED_CB_ID, []( TIM_HandleTypeDef* ) {
                            HBRIDGE.timer_period_irq();
                    } );

                return res;
        };
        if ( !HBRIDGE.setup( setup_f ) ) {
                return nullptr;
        }

        return &HBRIDGE;
}

fw::comms* setup_comms()
{
        auto comms_setup = []( fw::comms::handles& h ) {
                return setup_iuart( h );
        };
        if ( !COMMS.setup( comms_setup ) ) {
                return nullptr;
        }
        return &COMMS;
}

fw::debug_comms* setup_debug_comms()
{
        auto setup_f = []( fw::debug_comms::handles& h ) {
                bool res = setup_duart( h );
                fw::hal_check{} << HAL_UART_RegisterCallback(
                    &h.uart, HAL_UART_RX_COMPLETE_CB_ID, []( UART_HandleTypeDef* huart ) {
                            DEBUG_COMMS.rx_cplt_irq( huart );
                    } );
                return res;
        };

        if ( !DEBUG_COMMS.setup( setup_f ) ) {
                return nullptr;
        }

        return &DEBUG_COMMS;
}

fw::leds* setup_leds()
{
        auto setup_f = []( fw::leds::handles& h ) {
                return setup_leds_gpio( h ) && setup_leds_timer( h );
        };

        if ( !LEDS.setup( setup_f ) ) {
                return nullptr;
        }

        return &LEDS;
}

}  // namespace brd
