#include "fw/board.hpp"

#include "setup.hpp"

namespace brd
{

fw::drv::clock       CLOCK{};
fw::drv::acquisition ACQUISITION{};
fw::drv::comms       COMMS{};
fw::drv::debug_comms DEBUG_COMMS{};
fw::drv::hbridge     HBRIDGE{};
fw::drv::leds        LEDS;

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

void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef* h )
{
        brd::HBRIDGE.timer_period_irq( h );
}

void HAL_UART_RxCpltCallback( UART_HandleTypeDef* h )
{
        brd::COMMS.rx_cplt_irq( h );
        brd::DEBUG_COMMS.rx_cplt_irq( h );
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

fw::drv::clock* setup_clock()
{
        auto clk_setup = []( fw::drv::clock::handles& h ) {
                __HAL_RCC_TIM2_CLK_ENABLE();
                return setup_clock_timer(
                    h,
                    clock_timer_cfg{
                        .timer_instance = TIM2,
                        .channel        = TIM_CHANNEL_1,
                    } );
        };

        if ( !CLOCK.setup( clk_setup ) ) {
                return nullptr;
        }
        return &CLOCK;
}

fw::drv::acquisition* setup_acquisition()
{
        auto acq_setup = []( fw::drv::acquisition::handles& h ) {
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
                                       .priority     = DMA_PRIORITY_VERY_HIGH,
                                   },
                               .current =
                                   pinch_cfg{
                                       .channel = ADC_CHANNEL_4,
                                       .pin     = GPIO_PIN_3,
                                       .port    = GPIOA,
                                   },
                               .position =
                                   pinch_cfg{
                                       .channel = ADC_CHANNEL_1,
                                       .pin     = GPIO_PIN_0,
                                       .port    = GPIOA,
                                   },
                               .vcc =
                                   pinch_cfg{
                                       .channel = ADC_CHANNEL_2,
                                       .pin     = GPIO_PIN_1,
                                       .port    = GPIOA,
                                   },
                               .temp =
                                   pinch_cfg{
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

fw::drv::hbridge* setup_hbridge()
{
        auto setup_f = []( fw::drv::hbridge::handles& h ) {
                __HAL_RCC_TIM1_CLK_ENABLE();
                __HAL_RCC_GPIOA_CLK_ENABLE();
                bool res = setup_hbridge_timers(
                    h,
                    hb_timer_cfg{
                        .timer_instance = TIM1,
                        .period         = std::numeric_limits< uint16_t >::max() / 8,
                        .irq            = TIM1_UP_TIM16_IRQn,
                        .irq_priority   = 0,
                        .mc1 =
                            pinch_cfg{
                                .channel   = TIM_CHANNEL_1,
                                .pin       = GPIO_PIN_8,
                                .port      = GPIOA,
                                .alternate = GPIO_AF6_TIM1,
                            },
                        .mc2 =
                            pinch_cfg{
                                .channel   = TIM_CHANNEL_2,
                                .pin       = GPIO_PIN_9,
                                .port      = GPIOA,
                                .alternate = GPIO_AF6_TIM1,
                            },
                    } );

                return res;
        };
        if ( !HBRIDGE.setup( setup_f ) ) {
                return nullptr;
        }

        return &HBRIDGE;
}

fw::drv::comms* setup_comms()
{
        auto comms_setup = []( fw::drv::comms::handles& h ) {
                __HAL_RCC_DMAMUX1_CLK_ENABLE();
                __HAL_RCC_DMA1_CLK_ENABLE();
                __HAL_RCC_USART2_CLK_ENABLE();
                __HAL_RCC_GPIOB_CLK_ENABLE();
                return setup_uart(
                    h.uart,
                    h.tx_dma,
                    uart_cfg{
                        .uart_instance = USART2,
                        .baudrate      = 115200,
                        .irq           = USART2_IRQn,
                        .irq_priority  = 1,
                        .rx =
                            pin_cfg{
                                .pin       = GPIO_PIN_4,
                                .port      = GPIOB,
                                .alternate = GPIO_AF7_USART2,
                            },
                        .tx =
                            pin_cfg{
                                .pin       = GPIO_PIN_3,
                                .port      = GPIOB,
                                .alternate = GPIO_AF7_USART2,
                            },
                        .tx_dma =
                            dma_cfg{
                                .instance     = DMA1_Channel4,
                                .irq          = DMA1_Channel4_IRQn,
                                .irq_priority = 1,
                                .request      = DMA_REQUEST_USART2_TX,
                                .priority     = DMA_PRIORITY_LOW,
                            },
                    } );
        };
        if ( !COMMS.setup( comms_setup ) ) {
                return nullptr;
        }
        return &COMMS;
}

fw::drv::debug_comms* setup_debug_comms()
{
        auto setup_f = []( fw::drv::debug_comms::handles& h ) {
                __HAL_RCC_DMA1_CLK_ENABLE();
                __HAL_RCC_DMAMUX1_CLK_ENABLE();
                __HAL_RCC_GPIOA_CLK_ENABLE();
                __HAL_RCC_GPIOB_CLK_ENABLE();
                __HAL_RCC_USART1_CLK_ENABLE();
                bool res = setup_uart(
                    h.uart,
                    h.tx_dma,
                    uart_cfg{
                        .uart_instance = USART1,
                        .baudrate      = 460800,
                        .irq           = USART1_IRQn,
                        .irq_priority  = 1,
                        .rx =
                            pin_cfg{
                                .pin       = GPIO_PIN_10,
                                .port      = GPIOA,
                                .alternate = GPIO_AF7_USART1,
                            },
                        .tx =
                            pin_cfg{
                                .pin       = GPIO_PIN_6,
                                .port      = GPIOB,
                                .alternate = GPIO_AF7_USART1,
                            },
                        .tx_dma =
                            dma_cfg{
                                .instance     = DMA1_Channel5,
                                .irq          = DMA1_Channel5_IRQn,
                                .irq_priority = 1,
                                .request      = DMA_REQUEST_USART1_TX,
                                .priority     = DMA_PRIORITY_LOW,
                            },
                    } );
                return res;
        };

        if ( !DEBUG_COMMS.setup( setup_f ) ) {
                return nullptr;
        }

        return &DEBUG_COMMS;
}

fw::drv::leds* setup_leds()
{
        auto setup_f = []( fw::drv::leds::handles& h ) {
                __HAL_RCC_GPIOF_CLK_ENABLE();
                __HAL_RCC_GPIOB_CLK_ENABLE();
                __HAL_RCC_TIM3_CLK_ENABLE();
                return setup_leds_gpio(
                           h,
                           leds_gpio_cfg{
                               .red =
                                   pin_cfg{
                                       .pin  = GPIO_PIN_0,
                                       .port = GPIOF,
                                   },
                               .blue =
                                   pin_cfg{
                                       .pin  = GPIO_PIN_1,
                                       .port = GPIOF,
                                   },
                           } ) &&
                       setup_leds_timer(
                           h,
                           leds_timer_cfg{
                               .timer_instance = TIM3,
                               .yellow =
                                   pinch_cfg{
                                       .channel   = TIM_CHANNEL_2,
                                       .pin       = GPIO_PIN_5,
                                       .port      = GPIOB,
                                       .alternate = GPIO_AF2_TIM3,
                                   },
                               .green =
                                   pinch_cfg{
                                       .channel   = TIM_CHANNEL_3,
                                       .pin       = GPIO_PIN_0,
                                       .port      = GPIOB,
                                       .alternate = GPIO_AF2_TIM3,
                                   },
                           } );
        };

        if ( !LEDS.setup( setup_f ) ) {
                return nullptr;
        }

        return &LEDS;
}

}  // namespace brd
