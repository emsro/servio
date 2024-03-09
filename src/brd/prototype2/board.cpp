#include "brd.hpp"
#include "core/drivers.hpp"
#include "core/globals.hpp"
#include "drv/adc_pooler.hpp"
#include "drv/adc_pooler_def.hpp"
#include "drv/clock.hpp"
#include "drv/cobs_uart.hpp"
#include "drv/leds.hpp"
#include "emlabcpp/result.h"
#include "fw/install_stop_callback.hpp"
#include "fw/util.hpp"
#include "setup.hpp"
#include "sntr/central_sentry.hpp"

namespace servio::brd
{

TIM_HandleTypeDef TIM2_HANDLE{};
drv::clock        CLOCK{ TIM2_HANDLE };

std::array< sntr::record, 16 >  INOPERABLE_RECORDS;
std::array< sntr::record, 128 > DEGRADED_RECORDS;

sntr::central_sentry CENTRAL_SENTRY{
    CLOCK,
    INOPERABLE_RECORDS,
    DEGRADED_RECORDS,
    core::STOP_CALLBACK };

ADC_HandleTypeDef ADC_HANDLE{};
DMA_HandleTypeDef ADC_DMA_HANDLE{};
TIM_HandleTypeDef ADC_TIM_HANDLE{};

using adc_pooler_type = drv::adc_pooler< drv::adc_set< CENTRAL_SENTRY > >;

adc_pooler_type ADC_POOLER{ drv::ADC_SEQUENCE, ADC_HANDLE, ADC_DMA_HANDLE, ADC_TIM_HANDLE };
drv::adc_pooler_period_cb< ADC_POOLER >   ADC_PERIOD_CB;
drv::adc_pooler_vcc< ADC_POOLER >         ADC_VCC;
drv::adc_pooler_temperature< ADC_POOLER > ADC_TEMPERATURE;
drv::adc_pooler_position< ADC_POOLER >    ADC_POSITION;
drv::adc_pooler_current< ADC_POOLER >     ADC_CURRENT;

UART_HandleTypeDef UART2_HANDLE{};
DMA_HandleTypeDef  UART2_DMA_HANDLE{};
drv::cobs_uart     COMMS{ "comms", CENTRAL_SENTRY, CLOCK, &UART2_HANDLE, &UART2_DMA_HANDLE };
UART_HandleTypeDef UART1_HANDLE{};
DMA_HandleTypeDef  UART1_DMA_HANDLE{};
drv::cobs_uart     DEBUG_COMMS{ "dcomms", CENTRAL_SENTRY, CLOCK, &UART1_HANDLE, &UART1_DMA_HANDLE };
TIM_HandleTypeDef  TIM1_HANDLE{};
drv::hbridge       HBRIDGE{ &TIM1_HANDLE };
TIM_HandleTypeDef  TIM3_HANDLE{};
drv::leds          LEDS{ TIM3_HANDLE };

}  // namespace servio::brd

extern "C" {

void TIM1_UP_TIM16_IRQHandler()
{
        HAL_TIM_IRQHandler( &servio::brd::TIM1_HANDLE );
}

void USART2_IRQHandler()
{
        HAL_UART_IRQHandler( &servio::brd::UART2_HANDLE );
}

void DMA1_Channel4_IRQHandler()
{
        // TODO: is this needed?
        HAL_DMA_IRQHandler( &servio::brd::UART2_DMA_HANDLE );
}

void USART1_IRQHandler()
{
        HAL_UART_IRQHandler( &servio::brd::UART1_HANDLE );
}

void DMA1_Channel5_IRQHandler()
{
        // TODO: is this needed?
        HAL_DMA_IRQHandler( &servio::brd::UART1_DMA_HANDLE );
}

[[gnu::flatten]] void DMA1_Channel1_IRQHandler()
{
        HAL_DMA_IRQHandler( &servio::brd::ADC_DMA_HANDLE );
}

[[gnu::flatten]] void ADC1_2_IRQHandler()
{
        HAL_ADC_IRQHandler( &servio::brd::ADC_HANDLE );
}

void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef* h )
{
        servio::brd::HBRIDGE.timer_period_irq( h );
}

void HAL_UART_TxCpltCallback( UART_HandleTypeDef* h )
{
        servio::brd::COMMS.tx_cplt_irq( h );
        servio::brd::DEBUG_COMMS.tx_cplt_irq( h );
}

[[gnu::flatten]] void HAL_UART_RxCpltCallback( UART_HandleTypeDef* h )
{
        servio::brd::COMMS.rx_cplt_irq( h );
        servio::brd::DEBUG_COMMS.rx_cplt_irq( h );
}

[[gnu::flatten]] void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* h )
{
        servio::brd::ADC_POOLER.adc_conv_cplt_irq( h );
}

[[gnu::flatten]] void HAL_ADC_ErrorCallback( ADC_HandleTypeDef* h )
{
        servio::brd::ADC_POOLER.adc_error_irq( h );
}
}

namespace servio::brd
{

em::result setup_board()
{
        if ( HAL_Init() != HAL_OK )
                return em::ERROR;
        setup_clk();

        return em::SUCCESS;
}

adc_pooler_type* adc_pooler_setup()
{
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM4_CLK_ENABLE();
        setup_adc_channel(
            ADC_POOLER->current.chconf,
            drv::pinch_cfg{
                .channel = ADC_CHANNEL_4,
                .pin     = GPIO_PIN_3,
                .port    = GPIOA,
            } );
        setup_adc_channel(
            ADC_POOLER->position.chconf,
            drv::pinch_cfg{
                .channel = ADC_CHANNEL_1,
                .pin     = GPIO_PIN_0,
                .port    = GPIOA,
            } );
        setup_adc_channel(
            ADC_POOLER->vcc.chconf,
            drv::pinch_cfg{
                .channel = ADC_CHANNEL_2,
                .pin     = GPIO_PIN_1,
                .port    = GPIOA,
            } );
        setup_adc_channel(
            ADC_POOLER->temp.chconf,
            drv::pinch_cfg{
                .channel = ADC_CHANNEL_TEMPSENSOR_ADC1,
                .pin     = 0,
                .port    = nullptr,
            } );
        em::result res = em::worst_of(
            setup_adc(
                ADC_HANDLE,
                ADC_DMA_HANDLE,
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
                } ),
            setup_adc_timer( ADC_TIM_HANDLE, TIM4 ) );

        if ( res != em::SUCCESS )
                return nullptr;

        return &ADC_POOLER;
}

drv::hbridge* hbridge_setup()
{
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        hb_timer_cfg cfg{
            .timer_instance = TIM1,
            .period         = std::numeric_limits< uint16_t >::max() / 8,
            .irq            = TIM1_UP_TIM16_IRQn,
            .irq_priority   = 0,
            .mc1 =
                drv::pinch_cfg{
                    .channel   = TIM_CHANNEL_1,
                    .pin       = GPIO_PIN_8,
                    .port      = GPIOA,
                    .alternate = GPIO_AF6_TIM1,
                },
            .mc2 =
                drv::pinch_cfg{
                    .channel   = TIM_CHANNEL_2,
                    .pin       = GPIO_PIN_9,
                    .port      = GPIOA,
                    .alternate = GPIO_AF6_TIM1,
                },
        };
        if ( setup_hbridge_timers( TIM1_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return HBRIDGE.setup( cfg.mc1.channel, cfg.mc2.channel );
}

drv::cobs_uart* comms_setup()
{
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        uart_cfg cfg{
            .uart_instance = USART2,
            .baudrate      = 115200,
            .irq           = USART2_IRQn,
            .irq_priority  = 1,
            .rx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_4,
                    .port      = GPIOB,
                    .alternate = GPIO_AF7_USART2,
                },
            .tx =
                drv::pin_cfg{
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
        };

        if ( setup_uart( UART2_HANDLE, UART2_DMA_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return &COMMS;
}

base::com_interface* setup_debug_comms()
{
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        uart_cfg cfg{
            .uart_instance = USART1,
            .baudrate      = 460800,
            .irq           = USART1_IRQn,
            .irq_priority  = 1,
            .rx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_10,
                    .port      = GPIOA,
                    .alternate = GPIO_AF7_USART1,
                },
            .tx =
                drv::pin_cfg{
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
        };

        if ( setup_uart( UART1_HANDLE, UART1_DMA_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return &DEBUG_COMMS;
}

drv::leds* leds_setup()
{
        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM3_CLK_ENABLE();

        drv::pin_cfg red{
            .pin  = GPIO_PIN_0,
            .port = GPIOF,
        };
        drv::pin_cfg blue{
            .pin  = GPIO_PIN_1,
            .port = GPIOF,
        };
        drv::pinch_cfg yellow{
            .channel   = TIM_CHANNEL_2,
            .pin       = GPIO_PIN_5,
            .port      = GPIOB,
            .alternate = GPIO_AF2_TIM3,
        };
        drv::pinch_cfg green{
            .channel   = TIM_CHANNEL_3,
            .pin       = GPIO_PIN_0,
            .port      = GPIOB,
            .alternate = GPIO_AF2_TIM3,
        };

        em::result res = em::worst_of(
            setup_gpio( red ),
            setup_gpio( blue ),
            setup_leds_timer( TIM3_HANDLE, TIM3 ),
            setup_leds_channel( TIM3_HANDLE, yellow ),
            setup_leds_channel( TIM3_HANDLE, green ) );

        if ( res == em::SUCCESS )
                return LEDS.setup( red, blue, yellow, green );

        return nullptr;
}

em::result start_callback( core::drivers& cdrv )
{

        if ( cdrv.position != nullptr ) {
                // this implies that adc_poolerition is OK
                if ( ADC_POOLER.start() != em::SUCCESS )
                        return em::ERROR;
        }
        if ( cdrv.motor != nullptr ) {
                if ( HBRIDGE.start() != em::SUCCESS )
                        return em::ERROR;
        }
        if ( cdrv.comms != nullptr ) {
                if ( COMMS.start() != em::SUCCESS )
                        return em::ERROR;
        }
        if ( cdrv.leds != nullptr ) {
                if ( LEDS.start() != em::SUCCESS )
                        return em::ERROR;
        }

        return em::SUCCESS;
}

core::drivers setup_core_drivers()
{
        __HAL_RCC_TIM2_CLK_ENABLE();
        if ( setup_clock_timer( TIM2_HANDLE, TIM2 ) != em::SUCCESS )
                fw::stop_exec();  // TODO: better error handling

        drv::hbridge* hb = hbridge_setup();

        auto*      adc_pooler = adc_pooler_setup();
        drv::leds* leds       = leds_setup();
        if ( hb != nullptr )
                fw::install_stop_callback( core::STOP_CALLBACK, *hb, leds );

        return core::drivers{
            .clock       = &CLOCK,
            .position    = adc_pooler == nullptr ? nullptr : &ADC_POSITION,
            .current     = adc_pooler == nullptr ? nullptr : &ADC_CURRENT,
            .vcc         = adc_pooler == nullptr ? nullptr : &ADC_VCC,
            .temperature = adc_pooler == nullptr ? nullptr : &ADC_TEMPERATURE,
            .period_cb   = adc_pooler == nullptr ? nullptr : &ADC_PERIOD_CB,
            .motor       = hb,
            .period      = hb,
            .comms       = comms_setup(),
            .leds        = leds,
            .start_cb    = start_callback,
        };
}

}  // namespace servio::brd
