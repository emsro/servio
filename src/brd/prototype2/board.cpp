#include "brd.hpp"
#include "core/drivers.hpp"
#include "core/globals.hpp"
#include "drv/adc_pooler.hpp"
#include "drv/adc_pooler_def.hpp"
#include "drv/clock.hpp"
#include "drv/cobs_uart.hpp"
#include "drv/leds.hpp"
#include "emlabcpp/result.h"
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

struct adc_set
{
        using id_type = drv::chan_ids;

        drv::detailed_adc_channel< drv::CURRENT_CHANNEL, 128 > current{
            { "current", CENTRAL_SENTRY } };
        drv::adc_channel_with_callback< drv::POSITION_CHANNEL > position{
            "position",
            CENTRAL_SENTRY };
        drv::adc_channel< drv::VCC_CHANNEL >  vcc{ { "vcc", CENTRAL_SENTRY } };
        drv::adc_channel< drv::TEMP_CHANNEL > temp{ { "temp", CENTRAL_SENTRY } };

        [[gnu::flatten]] auto tie()
        {
                return std::tie( current, position, vcc, temp );
        }
};

using adc_pooler_type = drv::adc_pooler< adc_set >;

adc_pooler_type                           ADC_POOLER{ ADC_HANDLE, ADC_DMA_HANDLE, ADC_TIM_HANDLE };
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
drv::leds          LEDS{ &TIM3_HANDLE };

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
            ADC_CHANNEL_4,
            drv::pin_cfg{
                .pin  = GPIO_PIN_3,
                .port = GPIOA,
                .mode = GPIO_MODE_ANALOG,
            } );
        setup_adc_channel(
            ADC_POOLER->position.chconf,
            ADC_CHANNEL_1,
            drv::pin_cfg{
                .pin  = GPIO_PIN_0,
                .port = GPIOA,
                .mode = GPIO_MODE_ANALOG,
            } );
        setup_adc_channel(
            ADC_POOLER->vcc.chconf,
            ADC_CHANNEL_2,
            drv::pin_cfg{
                .pin  = GPIO_PIN_1,
                .port = GPIOA,
                .mode = GPIO_MODE_ANALOG,
            } );
        setup_adc_channel( ADC_POOLER->temp.chconf, ADC_CHANNEL_TEMPSENSOR_ADC1, std::nullopt );
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

        ADC_POOLER.set_seq( drv::ADC_FULL_SEQUENCE );

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
            .mc1_chan       = TIM_CHANNEL_1,
            .mc1_pin =
                {
                    .pin       = GPIO_PIN_8,
                    .port      = GPIOA,
                    .mode      = GPIO_MODE_AF_PP,
                    .alternate = GPIO_AF6_TIM1,
                },
            .mc2_chan = TIM_CHANNEL_2,
            .mc2_pin =
                {
                    .pin       = GPIO_PIN_9,
                    .port      = GPIOA,
                    .mode      = GPIO_MODE_AF_PP,
                    .alternate = GPIO_AF6_TIM1,
                },
        };
        if ( setup_hbridge_timers( TIM1_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return HBRIDGE.setup( cfg.mc1_chan, cfg.mc2_chan );
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

drv::com_iface* setup_debug_comms()
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

        setup_gpio( red );
        setup_gpio( blue );

        uint32_t     yellow_ch = TIM_CHANNEL_2;
        drv::pin_cfg yellow{
            .pin       = GPIO_PIN_5,
            .port      = GPIOB,
            .mode      = GPIO_MODE_AF_PP,
            .alternate = GPIO_AF2_TIM3,
        };
        uint32_t     green_ch = TIM_CHANNEL_3;
        drv::pin_cfg green{
            .pin       = GPIO_PIN_0,
            .port      = GPIOB,
            .mode      = GPIO_MODE_AF_PP,
            .alternate = GPIO_AF2_TIM3,
        };

        em::result res = em::worst_of(
            setup_leds_timer( TIM3_HANDLE, TIM3 ),
            setup_leds_channel( TIM3_HANDLE, yellow_ch, yellow ),
            setup_leds_channel( TIM3_HANDLE, green_ch, green ) );

        if ( res == em::SUCCESS )
                return LEDS.setup( red, blue, yellow_ch, green_ch );

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

void install_stop_callback( drv::pwm_motor_iface& motor, drv::leds_iface* leds_ptr )
{
        core::STOP_CALLBACK = [&motor, leds_ptr] {
                motor.force_stop();
                if ( leds_ptr != nullptr )
                        leds_ptr->force_red_led();
        };
}

core::drivers setup_core_drivers( const cfg::map& )
{
        __HAL_RCC_TIM2_CLK_ENABLE();
        if ( setup_clock_timer( TIM2_HANDLE, TIM2 ) != em::SUCCESS )
                fw::stop_exec();  // TODO: better error handling

        drv::hbridge* hb = hbridge_setup();

        auto*      adc_pooler = adc_pooler_setup();
        drv::leds* leds       = leds_setup();
        if ( hb != nullptr )
                install_stop_callback( *hb, leds );

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
