#include "brd.hpp"
#include "cfg/default.hpp"
#include "core/drivers.hpp"
#include "core/globals.hpp"
#include "drv/adc_pooler_def.hpp"
#include "drv/clock.hpp"
#include "drv/cobs_uart.hpp"
#include "drv/hbridge.hpp"
#include "drv/leds.hpp"
#include "fw/install_stop_callback.hpp"
#include "fw/util.hpp"
#include "platform.hpp"
#include "setup.hpp"
#include "sntr/central_sentry.hpp"

#include <emlabcpp/result.h>

namespace em = emlabcpp;

// TODO: this needs it's own header
extern "C" {
extern int _config_start;
extern int _config_end;
}

namespace servio::brd
{

TIM_HandleTypeDef TIM2_HANDLE = {};
drv::clock        CLOCK{ TIM2_HANDLE };
drv::leds         LEDS{ &TIM2_HANDLE };

std::array< sntr::record, 16 >  INOPERABLE_RECORDS;
std::array< sntr::record, 128 > DEGRADED_RECORDS;

sntr::central_sentry CENTRAL_SENTRY{
    CLOCK,
    INOPERABLE_RECORDS,
    DEGRADED_RECORDS,
    core::STOP_CALLBACK };

ADC_HandleTypeDef ADC_HANDLE;
DMA_HandleTypeDef ADC_DMA_HANDLE;
TIM_HandleTypeDef TIM6_HANDLE;

using adc_pooler_type = drv::adc_pooler< drv::adc_set< CENTRAL_SENTRY > >;
adc_pooler_type ADC_POOLER{ drv::ADC_SEQUENCE, ADC_HANDLE, ADC_DMA_HANDLE, TIM6_HANDLE };
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

}  // namespace servio::brd

extern "C" {

void ADC1_IRQHandler()
{
        HAL_ADC_IRQHandler( &servio::brd::ADC_HANDLE );
}

void TIM1_UP_IRQHandler()
{
        HAL_TIM_IRQHandler( &servio::brd::TIM1_HANDLE );
}

void TIM2_IRQHandler()
{
        HAL_TIM_IRQHandler( &servio::brd::TIM2_HANDLE );
}

void USART2_IRQHandler()
{
        HAL_UART_IRQHandler( &servio::brd::UART2_HANDLE );
}

void USART1_IRQHandler()
{
        HAL_UART_IRQHandler( &servio::brd::UART1_HANDLE );
}

void GPDMA1_Channel0_IRQHandler()
{
        HAL_DMA_IRQHandler( &servio::brd::ADC_DMA_HANDLE );
}

void GPDMA1_Channel1_IRQHandler()
{
        HAL_DMA_IRQHandler( &servio::brd::UART2_DMA_HANDLE );
}

void GPDMA1_Channel2_IRQHandler()
{
        HAL_DMA_IRQHandler( &servio::brd::UART1_DMA_HANDLE );
}
}

extern "C" {

[[gnu::flatten]] void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef* h )
{
        servio::brd::HBRIDGE.timer_period_irq( h );
        servio::brd::CLOCK.timer_period_irq( h );
}

[[gnu::flatten]] void HAL_UART_TxCpltCallback( UART_HandleTypeDef* h )
{
        servio::brd::COMMS.tx_cplt_irq( h );
        servio::brd::DEBUG_COMMS.tx_cplt_irq( h );
}

[[gnu::flatten]] void HAL_UART_RxCpltCallback( UART_HandleTypeDef* h )
{
        servio::brd::COMMS.rx_cplt_irq( h );
        servio::brd::DEBUG_COMMS.rx_cplt_irq( h );
}

[[gnu::flatten]] void HAL_UART_ErrorCallback( UART_HandleTypeDef* h )
{
        servio::brd::COMMS.err_irq( h );
        servio::brd::DEBUG_COMMS.err_irq( h );
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

adc_pooler_type* adc_pooler_setup()
{
        __HAL_RCC_ADC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPDMA1_CLK_ENABLE();
        __HAL_RCC_TIM6_CLK_ENABLE();
        plt::setup_adc_channel(
            ADC_POOLER->current.chconf,
            drv::pinch_cfg{
                .channel = ADC_CHANNEL_15,
                .pin     = GPIO_PIN_3,
                .port    = GPIOA,
            } );
        plt::setup_adc_channel(
            ADC_POOLER->position.chconf,
            drv::pinch_cfg{
                .channel = ADC_CHANNEL_0,
                .pin     = GPIO_PIN_0,
                .port    = GPIOA,
            } );
        plt::setup_adc_channel(
            ADC_POOLER->vcc.chconf,
            drv::pinch_cfg{
                .channel = ADC_CHANNEL_7,
                .pin     = GPIO_PIN_7,
                .port    = GPIOA,
            } );
        // TODO: temperature sensors was set to a random unused channel. We can't use the internal
        // temperature channel until the HAL get's cahnged:
        // https://github.com/STMicroelectronics/stm32h5xx_hal_driver/issues/4
        plt::setup_adc_channel(
            ADC_POOLER->temp.chconf,
            drv::pinch_cfg{
                .channel = ADC_CHANNEL_1,
                .pin     = 0,
                .port    = nullptr,
            } );
        em::result res = em::worst_of(
            plt::setup_adc(
                ADC_HANDLE,
                ADC_DMA_HANDLE,
                plt::adc_cfg{
                    .adc_instance     = ADC1,
                    .adc_irq_priority = 1,
                    .adc_trigger      = ADC_EXTERNALTRIG_T6_TRGO,
                    .dma =
                        plt::dma_cfg{
                            .irq          = GPDMA1_Channel0_IRQn,
                            .irq_priority = 1,
                            .instance     = GPDMA1_Channel0,
                            .request      = GPDMA1_REQUEST_ADC1,
                            .priority     = DMA_HIGH_PRIORITY,
                        },
                } ),
            plt::setup_adc_timer( TIM6_HANDLE, TIM6 ) );

        if ( res != em::SUCCESS )
                return nullptr;

        return &ADC_POOLER;
}

drv::hbridge* hbridge_setup()
{
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        plt::hb_timer_cfg cfg{
            .timer_instance = TIM1,
            .period         = std::numeric_limits< uint16_t >::max() / 8,
            .irq            = TIM1_UP_IRQn,
            .irq_priority   = 1,
            .mc1 =
                drv::pinch_cfg{
                    .channel   = TIM_CHANNEL_1,
                    .pin       = GPIO_PIN_6,
                    .port      = GPIOB,
                    .alternate = GPIO_AF14_TIM1,
                },
            .mc2 =
                drv::pinch_cfg{
                    .channel   = TIM_CHANNEL_2,
                    .pin       = GPIO_PIN_7,
                    .port      = GPIOB,
                    .alternate = GPIO_AF14_TIM1,
                },
        };
        if ( plt::setup_hbridge_timers( TIM1_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return HBRIDGE.setup( cfg.mc1.channel, cfg.mc2.channel );
}

drv::cobs_uart* comms_setup()
{
        __HAL_RCC_GPDMA1_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        plt::uart_cfg cfg{
            .uart_instance = USART2,
            .baudrate      = 460800,
            .irq           = USART2_IRQn,
            .irq_priority  = 2,
            .rx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_15,
                    .port      = GPIOB,
                    .alternate = GPIO_AF13_USART2,
                },
            .tx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_8,
                    .port      = GPIOA,
                    .alternate = GPIO_AF4_USART2,
                },
            .tx_dma =
                plt::dma_cfg{
                    .irq          = GPDMA1_Channel1_IRQn,
                    .irq_priority = 2,
                    .instance     = GPDMA1_Channel1,
                    .request      = GPDMA1_REQUEST_USART2_TX,
                    .priority     = DMA_LOW_PRIORITY_LOW_WEIGHT,
                },
        };

        if ( setup_uart( UART2_HANDLE, UART2_DMA_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return &COMMS;
}

drv::com_interface* setup_debug_comms()
{
        __HAL_RCC_GPDMA1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        plt::uart_cfg cfg{
            .uart_instance = USART1,
            .baudrate      = 460800,
            .irq           = USART1_IRQn,
            .irq_priority  = 2,
            .rx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_11,
                    .port      = GPIOA,
                    .alternate = GPIO_AF8_USART1,
                },
            .tx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_12,
                    .port      = GPIOA,
                    .alternate = GPIO_AF8_USART1,
                },
            .tx_dma =
                plt::dma_cfg{
                    .irq          = GPDMA1_Channel2_IRQn,
                    .irq_priority = 2,
                    .instance     = GPDMA1_Channel2,
                    .request      = GPDMA1_REQUEST_USART1_TX,
                    .priority     = DMA_LOW_PRIORITY_LOW_WEIGHT,
                },
        };

        if ( setup_uart( UART1_HANDLE, UART1_DMA_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return &DEBUG_COMMS;
}

drv::leds* leds_setup()
{
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();

        drv::pin_cfg red{
            .pin  = GPIO_PIN_13,
            .port = GPIOC,
        };
        drv::pin_cfg blue{
            .pin  = GPIO_PIN_14,
            .port = GPIOC,
        };
        drv::pinch_cfg yellow{
            .channel   = TIM_CHANNEL_3,
            .pin       = GPIO_PIN_10,
            .port      = GPIOB,
            .alternate = GPIO_AF1_TIM2,
        };

        drv::pinch_cfg green{
            .channel   = TIM_CHANNEL_1,
            .pin       = GPIO_PIN_2,
            .port      = GPIOB,
            .alternate = GPIO_AF14_TIM2,
        };

        em::result res = em::worst_of(
            plt::setup_gpio( red ),
            plt::setup_gpio( blue ),
            plt::setup_leds_channel( &TIM2_HANDLE, yellow ),
            plt::setup_leds_channel( &TIM2_HANDLE, green ) );

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

em::result setup_board()
{
        if ( HAL_Init() != HAL_OK )
                return em::ERROR;
        return plt::setup_clk();
}

// TODO: well, this was kinda duplicated...
core::drivers setup_core_drivers()
{
        __HAL_RCC_TIM2_CLK_ENABLE();
        if ( plt::setup_clock_timer( TIM2_HANDLE, TIM2, TIM2_IRQn ) != em::SUCCESS )
                fw::stop_exec();

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

cfg::map get_default_config()
{
        return plt::get_default_config();
}

// TODO: this needs better placement
// TODO: study this: https://github.com/littlefs-project/littlefs
em::view< std::byte* > page_at( uint32_t i )
{
        return em::view_n(
            reinterpret_cast< std::byte* >( &_config_start ) + i * FLASH_SECTOR_SIZE,
            FLASH_SECTOR_SIZE );
}

// TODO: one block is small, need more
std::array< em::view< std::byte* >, 1 > PERSISTENT_BLOCKS{ page_at( 0 ) };

em::view< const page* > get_persistent_pages()
{
        return PERSISTENT_BLOCKS;
}

}  // namespace servio::brd
