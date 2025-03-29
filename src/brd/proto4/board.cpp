#include "../../cfg/default.hpp"
#include "../../cnv/setup.hpp"
#include "../../core/drivers.hpp"
#include "../../core/globals.hpp"
#include "../../drv/adc_pooler_def.hpp"
#include "../../drv/char_uart.hpp"
#include "../../drv/clock.hpp"
#include "../../drv/cobs_uart.hpp"
#include "../../drv/dts_temp.hpp"
#include "../../drv/flash_cfg.hpp"
#include "../../drv/hbridge.hpp"
#include "../../drv/leds.hpp"
#include "../../drv/paged_i2c_eeprom.hpp"
#include "../../drv/quad_encoder.hpp"
#include "../../fw/load_persistent_config.hpp"
#include "../../fw/util.hpp"
#include "../../plt/platform.hpp"
#include "../../sntr/central_sentry.hpp"
#include "../brd.hpp"
#include "flash_cfg.hpp"
#include "setup.hpp"

#include <emlabcpp/defer.h>
#include <emlabcpp/result.h>

namespace em = emlabcpp;

// TODO: check duplication with proto3/board.cpp
// TODO: add calibration procedure for ADC1

namespace servio::brd
{

using namespace literals;

drv::pin_cfg HBRDIGE_VREF_PIN{
    .pin  = GPIO_PIN_4,
    .port = GPIOA,
    .pull = GPIO_PULLDOWN,
};

// XXX: duplicated and actually property of our hbridge
consteval cnv::off_scale get_curr_coeff()
{
        // mirror scale: 1575 uA/A
        // resistor: 1k
        constexpr float gain = 1'575.F / 1'000'000.F;
        return cnv::calc_current_conversion( 3.3F, 0.0F, 1 << 12, 1'000.F, gain );
}

cfg::map get_default_config()
{
        cfg::map m = plt::get_default_config();

        cnv::off_scale const curr_cfg = get_curr_coeff();

        m.set_val< cfg::key::CURRENT_CONV_SCALE >( curr_cfg.scale );
        m.set_val< cfg::key::CURRENT_CONV_OFFSET >( curr_cfg.offset );
        m.set_val< cfg::key::TEMP_CONV_OFFSET >( 0.0F );
        m.set_val< cfg::key::TEMP_CONV_SCALE >( 1.0F );

        return m;
}

cfg::context CFG{ .map = get_default_config() };

TIM_HandleTypeDef TIM2_HANDLE = {};
drv::clock        CLOCK{ TIM2_HANDLE };
drv::leds         LEDS{ &TIM2_HANDLE };

sntr::record INOPERABLE_RECORDS[16];
sntr::record DEGRADED_RECORDS[128];

sntr::central_sentry CENTRAL_SENTRY{
    CLOCK,
    INOPERABLE_RECORDS,
    DEGRADED_RECORDS,
    core::STOP_CALLBACK };

ADC_HandleTypeDef ADC_HANDLE;
DMA_HandleTypeDef GPDMA1_Ch0_HANDLE;
TIM_HandleTypeDef TIM6_HANDLE;

struct adc_set
{
        using id_type = drv::chan_ids;

        drv::detailed_adc_channel< drv::CURRENT_CHANNEL, 128 > current{
            { "current", CENTRAL_SENTRY } };
        drv::adc_channel_with_callback< drv::POSITION_CHANNEL > position{
            "position",
            CENTRAL_SENTRY };
        drv::adc_channel< drv::VCC_CHANNEL > vcc{ { "vcc", CENTRAL_SENTRY } };
};

using adc_pooler_type = drv::adc_pooler< adc_set >;
adc_pooler_type                         ADC_POOLER{ ADC_HANDLE, GPDMA1_Ch0_HANDLE, TIM6_HANDLE };
drv::adc_pooler_period_cb< ADC_POOLER > ADC_PERIOD_CB;
drv::adc_pooler_vcc< ADC_POOLER >       ADC_VCC;
drv::adc_pooler_position< ADC_POOLER >  ADC_POSITION;
drv::adc_pooler_current< ADC_POOLER >   ADC_CURRENT;

UART_HandleTypeDef UART2_HANDLE{};
DMA_HandleTypeDef  GPDMA1_Ch2_HANDLE{};
drv::cobs_uart DEBUG_COMMS{ "dcomms", CENTRAL_SENTRY, CLOCK, &UART2_HANDLE, &GPDMA1_Ch2_HANDLE };
UART_HandleTypeDef UART1_HANDLE{};
DMA_HandleTypeDef  GPDMA1_Ch1_HANDLE{};
drv::char_uart     COMMS{ "comms", CENTRAL_SENTRY, CLOCK, &UART1_HANDLE, &GPDMA1_Ch1_HANDLE };
TIM_HandleTypeDef  TIM1_HANDLE{};
drv::hbridge       HBRIDGE{ &TIM1_HANDLE };
DTS_HandleTypeDef  DTS_HANDLE{};
drv::dts_temp      DTS_DRV{ DTS_HANDLE };

TIM_HandleTypeDef  TIM3_HANDLE{};
drv::quad_encoder  QUAD{ TIM3_HANDLE };
TIM_HandleTypeDef* QUAD_TRIGGER_TIMER = nullptr;

I2C_HandleTypeDef I2C2_HANDLE{};
drv::i2c_eeprom   EEPROM{ 0b1010'0000, 65'535u, 2'000u, I2C2_HANDLE };

std::array< em::view< std::byte* >, 2 > PERSISTENT_BLOCKS{ page_at( 0 ), page_at( 1 ) };

void flash_start_cb()
{
        std::ignore = HBRIDGE.start();
}

void flash_stop_cb()
{
        std::ignore = HBRIDGE.stop();
}

drv::flash_storage FLASH_STORAGE{ PERSISTENT_BLOCKS, flash_start_cb, flash_stop_cb };

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
        HAL_DMA_IRQHandler( &servio::brd::GPDMA1_Ch0_HANDLE );
}

void GPDMA1_Channel1_IRQHandler()
{
        HAL_DMA_IRQHandler( &servio::brd::GPDMA1_Ch1_HANDLE );
}

void GPDMA1_Channel2_IRQHandler()
{
        HAL_DMA_IRQHandler( &servio::brd::GPDMA1_Ch2_HANDLE );
}

void I2C2_ER_IRQHandler()
{
        HAL_I2C_ER_IRQHandler( &servio::brd::I2C2_HANDLE );
}

void I2C2_EV_IRQHandler()
{
        HAL_I2C_EV_IRQHandler( &servio::brd::I2C2_HANDLE );
}
}

extern "C" {

[[gnu::flatten]] void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef* h )
{
        servio::brd::HBRIDGE.timer_period_irq( h );
        servio::brd::CLOCK.timer_period_irq( h );

        if ( h == servio::brd::QUAD_TRIGGER_TIMER )
                servio::brd::QUAD.on_tick_irq();
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

adc_pooler_type* adc_pooler_setup( bool enable_pos )
{
        __HAL_RCC_ADC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPDMA1_CLK_ENABLE();
        __HAL_RCC_TIM6_CLK_ENABLE();
        plt::setup_adc_channel(
            ADC_POOLER->current.chconf,
            ADC_CHANNEL_15,
            drv::pin_cfg{
                .pin  = GPIO_PIN_3,
                .port = GPIOA,
                .mode = GPIO_MODE_ANALOG,
            } );
        plt::setup_adc_channel(
            ADC_POOLER->position.chconf,
            ADC_CHANNEL_19,
            drv::pin_cfg{
                .pin  = GPIO_PIN_5,
                .port = GPIOA,
                .mode = GPIO_MODE_ANALOG,
            } );
        plt::setup_adc_channel(
            ADC_POOLER->vcc.chconf,
            ADC_CHANNEL_7,
            drv::pin_cfg{
                .pin  = GPIO_PIN_7,
                .port = GPIOA,
                .mode = GPIO_MODE_ANALOG,
            } );
        em::result res = plt::setup_adc(
                             ADC_HANDLE,
                             GPDMA1_Ch0_HANDLE,
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
                             } ) &&
                         plt::setup_adc_timer( TIM6_HANDLE, TIM6 );

        if ( res != em::SUCCESS )
                return nullptr;

        if ( enable_pos )
                ADC_POOLER.set_seq( drv::ADC_CURR_POS_VCC_SEQUENCE );
        else
                ADC_POOLER.set_seq( drv::ADC_CURR_VCC_SEQUENCE );

        return &ADC_POOLER;
}

drv::hbridge* hbridge_setup()
{
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        plt::hb_timer_cfg cfg{
            .timer_instance = TIM1,
            .period         = std::numeric_limits< uint16_t >::max() / 4,
            .irq            = TIM1_UP_IRQn,
            .irq_priority   = 1,
            .mc1_ch         = TIM_CHANNEL_1,
            .mc1_pin =
                {
                    .pin       = GPIO_PIN_6,
                    .port      = GPIOB,
                    .mode      = GPIO_MODE_AF_PP,
                    .alternate = GPIO_AF14_TIM1,
                },
            .mc2_ch = TIM_CHANNEL_2,
            .mc2_pin =
                {
                    .pin       = GPIO_PIN_7,
                    .port      = GPIOB,
                    .mode      = GPIO_MODE_AF_PP,
                    .alternate = GPIO_AF14_TIM1,
                },
        };
        if ( plt::setup_hbridge_timers( TIM1_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return HBRIDGE.setup( cfg.mc1_ch, cfg.mc2_ch );
}

drv::com_iface* comms_setup()
{
        __HAL_RCC_GPDMA1_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        plt::uart_cfg cfg{
            .uart_instance = USART1,
            .baudrate      = 230400,
            .irq           = USART1_IRQn,
            .irq_priority  = 0,
            .rx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_10,
                    .port      = GPIOA,
                    .mode      = GPIO_MODE_AF_PP,
                    .alternate = GPIO_AF7_USART1,
                },
            .tx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_9,
                    .port      = GPIOA,
                    .mode      = GPIO_MODE_AF_PP,
                    .alternate = GPIO_AF7_USART1,
                },
            .tx_dma =
                plt::dma_cfg{
                    .irq          = GPDMA1_Channel1_IRQn,
                    .irq_priority = 0,
                    .instance     = GPDMA1_Channel1,
                    .request      = GPDMA1_REQUEST_USART1_TX,
                    .priority     = DMA_LOW_PRIORITY_LOW_WEIGHT,
                },
        };

        if ( setup_uart( UART1_HANDLE, GPDMA1_Ch1_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return &COMMS;
}

drv::com_iface* setup_debug_comms()
{
        __HAL_RCC_GPDMA1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();

        plt::uart_cfg cfg{
            .uart_instance = USART2,
            .baudrate      = 230400,
            .irq           = USART2_IRQn,
            .irq_priority  = 0,
            .rx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_11,
                    .port      = GPIOA,
                    .mode      = GPIO_MODE_AF_PP,
                    .alternate = GPIO_AF4_USART2,
                },
            .tx =
                drv::pin_cfg{
                    .pin       = GPIO_PIN_12,
                    .port      = GPIOA,
                    .mode      = GPIO_MODE_AF_PP,
                    .alternate = GPIO_AF4_USART2,
                },
            .tx_dma =
                plt::dma_cfg{
                    .irq          = GPDMA1_Channel2_IRQn,
                    .irq_priority = 0,
                    .instance     = GPDMA1_Channel2,
                    .request      = GPDMA1_REQUEST_USART2_TX,
                    .priority     = DMA_LOW_PRIORITY_LOW_WEIGHT,
                },
        };

        if ( setup_uart( UART2_HANDLE, GPDMA1_Ch2_HANDLE, cfg ) != em::SUCCESS )
                return nullptr;

        return &DEBUG_COMMS;
}

drv::leds* leds_setup()
{
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();

        drv::pin_cfg red{
            .pin  = GPIO_PIN_0,
            .port = GPIOA,
        };
        drv::pin_cfg blue{
            .pin  = GPIO_PIN_1,
            .port = GPIOA,
        };
        plt::setup_gpio( red );
        plt::setup_gpio( blue );
        // XXX: in theory, red/blue could also be on TIM2
        uint32_t     green_ch = TIM_CHANNEL_3;
        drv::pin_cfg green{
            .pin       = GPIO_PIN_2,
            .port      = GPIOA,
            .mode      = GPIO_MODE_AF_PP,
            .alternate = GPIO_AF1_TIM2,
        };

        em::result res = plt::setup_leds_channel( &TIM2_HANDLE, green_ch, green );

        if ( res == em::SUCCESS )
                return LEDS.setup( red, blue, green_ch );

        return nullptr;
}

drv::pos_iface* quad_encoder_setup( uint32_t period )
{

        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        drv::pin_cfg ch1 = {
            .pin       = GPIO_PIN_4,
            .port      = GPIOB,
            .mode      = GPIO_MODE_AF_PP,
            .alternate = GPIO_AF2_TIM3,
        };
        plt::setup_gpio( ch1 );

        drv::pin_cfg ch2 = {
            .pin       = GPIO_PIN_5,
            .port      = GPIOB,
            .mode      = GPIO_MODE_AF_PP,
            .alternate = GPIO_AF2_TIM3,
        };
        plt::setup_gpio( ch2 );

        // XXX: technicaly we are missing PA8 - encoder clk - indication of zero
        em::result res = plt::setup_encoder_timer( TIM3_HANDLE, TIM3, period );
        if ( res != em::SUCCESS )
                return nullptr;

        QUAD_TRIGGER_TIMER = &TIM1_HANDLE;

        return &QUAD;
}

void try_i2c_gpio_toggle()
{
        drv::pin_cfg a{
            .pin  = GPIO_PIN_10,
            .port = GPIOB,
            .mode = GPIO_MODE_OUTPUT_OD,
            .pull = GPIO_PULLUP,
        };
        drv::pin_cfg b{
            .pin  = GPIO_PIN_13,
            .port = GPIOB,
            .mode = GPIO_MODE_OUTPUT_OD,
            .pull = GPIO_PULLUP,
        };
        plt::setup_gpio( a );
        plt::setup_gpio( b );
        for ( ;; ) {
                HAL_GPIO_TogglePin( GPIOB, a.pin );
                HAL_GPIO_TogglePin( GPIOB, b.pin );
                drv::wait_for( CLOCK, 10_ms );
        }
}

drv::storage_iface* eeprom_setup()
{
        __HAL_RCC_GPIOB_CLK_ENABLE();

        drv::pin_cfg pwr{
            .pin  = GPIO_PIN_15,
            .port = GPIOB,
            .mode = GPIO_MODE_OUTPUT_PP,
        };
        plt::setup_gpio( pwr );
        HAL_GPIO_WritePin( pwr.port, pwr.pin, GPIO_PIN_SET );

        plt::i2c_cfg cfg{
            .instance        = I2C2,
            .ev_irq          = I2C2_EV_IRQn,
            .ev_irq_priority = 4,
            .er_irq          = I2C2_ER_IRQn,
            .er_irq_priority = 4,
            .scl =
                {
                    .pin       = GPIO_PIN_10,
                    .port      = GPIOB,
                    .mode      = GPIO_MODE_AF_OD,
                    .alternate = GPIO_AF4_I2C2,
                    .pull      = GPIO_PULLUP,
                },
            .sda = {
                .pin       = GPIO_PIN_13,
                .port      = GPIOB,
                .mode      = GPIO_MODE_AF_OD,
                .alternate = GPIO_AF4_I2C2,
                .pull      = GPIO_PULLUP,
            } };

        em::result res = plt::setup_i2c( I2C2_HANDLE, cfg );
        if ( res != em::SUCCESS )
                return nullptr;

        __HAL_RCC_I2C2_CLK_ENABLE();

        return &EEPROM;
}

em::result start_callback( core::drivers& cdrv )
{
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
        if ( cdrv.storage == &EEPROM ) {
                if ( EEPROM.start() != em::SUCCESS )
                        return em::ERROR;
        }
        if ( QUAD_TRIGGER_TIMER != nullptr )
                QUAD.start();

        HAL_GPIO_WritePin( HBRDIGE_VREF_PIN.port, HBRDIGE_VREF_PIN.pin, GPIO_PIN_SET );

        drv::wait_for( CLOCK, 50_us );

        if ( HAL_ADCEx_Calibration_Start( &ADC_HANDLE, ADC_SINGLE_ENDED ) != HAL_OK )
                return em::ERROR;

        if ( cdrv.position != nullptr ) {
                // this implies that adc_pooler initialization is OK
                if ( ADC_POOLER.start() != em::SUCCESS )
                        return em::ERROR;
        }

        if ( HAL_ICACHE_Enable() != HAL_OK )
                return em::ERROR;

        return em::SUCCESS;
}

void install_stop_callback( drv::pwm_motor_iface& motor, drv::leds_iface* leds_ptr )
{
        core::STOP_CALLBACK = [&motor, leds_ptr] {
                motor.force_stop();
                HAL_GPIO_WritePin( HBRDIGE_VREF_PIN.port, HBRDIGE_VREF_PIN.pin, GPIO_PIN_RESET );
                if ( leds_ptr != nullptr )
                        leds_ptr->force_red_led();
        };
}

em::result setup_board()
{
        if ( HAL_Init() != HAL_OK )
                return em::ERROR;
        return plt::setup_clk();
}

core::drivers setup_core_drivers()
{
        CFG.payload = fw::load_persistent_config( FLASH_STORAGE, CFG.map );

        __HAL_RCC_TIM2_CLK_ENABLE();
        if ( plt::setup_clock_timer( TIM2_HANDLE, TIM2, TIM2_IRQn ) != em::SUCCESS )
                fw::stop_exec();

        __HAL_RCC_DTS_CLK_ENABLE();
        if ( plt::setup_dts( DTS_HANDLE, DTS ) != em::SUCCESS )
                fw::stop_exec();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        plt::setup_gpio( HBRDIGE_VREF_PIN );

        drv::hbridge* hb = hbridge_setup();

        cfg::encoder_mode emod = CFG.map.get_val< cfg::ENCODER_MODE >();

        auto*           adc_pooler = adc_pooler_setup( emod == cfg::encoder_mode::ENC_MODE_ANALOG );
        drv::pos_iface* pos        = nullptr;
        if ( emod == 0 )
                fw::stop_exec();
        switch ( emod ) {
        case cfg::encoder_mode::ENC_MODE_ANALOG:
                pos = adc_pooler == nullptr ? nullptr : &ADC_POSITION;
                break;
        case cfg::encoder_mode::ENC_MODE_QUAD:
                pos = quad_encoder_setup( CFG.map.get_val< cfg::QUAD_ENCD_RANGE >() );
                break;
        }

        drv::leds* leds = leds_setup();
        if ( hb != nullptr )
                install_stop_callback( *hb, leds );

        return core::drivers{
            .cfg = &CFG,
            //.storage     = &FLASH_STORAGE,
            .storage     = eeprom_setup(),
            .clock       = &CLOCK,
            .position    = pos,
            .current     = adc_pooler == nullptr ? nullptr : &ADC_CURRENT,
            .vcc         = adc_pooler == nullptr ? nullptr : &ADC_VCC,
            .temperature = &DTS_DRV,
            .period_cb   = adc_pooler == nullptr ? nullptr : &ADC_PERIOD_CB,
            .motor       = hb,
            .period      = hb,
            .comms       = comms_setup(),
            .leds        = leds,
            .start_cb    = start_callback,
        };
}

}  // namespace servio::brd
