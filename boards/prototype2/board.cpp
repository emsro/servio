#include "brd.hpp"
#include "core_drivers.hpp"
#include "fw/drv/acquisition.hpp"
#include "fw/drv/clock.hpp"
#include "fw/drv/cobs_uart.hpp"
#include "fw/util.hpp"
#include "setup.hpp"

namespace brd
{

fw::drv::clock     CLOCK{};
acquisition_type   ACQUISITION{};
fw::drv::cobs_uart COMMS{};
fw::drv::cobs_uart DEBUG_COMMS{};
fw::drv::hbridge   HBRIDGE{};
fw::drv::leds      LEDS;

struct : vcc_interface
{
        status get_status() const override
        {
                return status::NOMINAL;
        }

        uint32_t get_vcc() const override
        {
                // TODO: well, the hardcoded constant is not ideal
                return ACQUISITION.get_val( 1 );
        }
} VCC;

struct : temperature_interface
{
        status get_status() const override
        {
                return status::NOMINAL;
        }

        uint32_t get_temperature() const override
        {
                // TODO: well, the hardcoded constant is not ideal
                return ACQUISITION.get_val( 2 );
        }
} TEMPERATURE;

struct : position_interface
{
        status get_status() const override
        {
                return status::NOMINAL;
        }

        uint32_t get_position() const override
        {
                // TODO: well, the hardcoded constant is not ideal
                return ACQUISITION.get_val( 3 );
        }

        void set_position_callback( position_cb_interface& cb ) override
        {
                ACQUISITION.set_brief_callback( 3, cb );
        }
        position_cb_interface& get_position_callback() const override
        {
                return ACQUISITION.get_brief_callback( 3 );
        }
} POSITION;

struct : current_interface
{
        status get_status() const override
        {
                status      res = status::NOMINAL;
                const auto& as  = ACQUISITION.status();
                if ( as.hal_start_failed ) {
                        res = status::DATA_ACQUISITION_CRITICAL_ERROR;
                } else if ( as.buffer_was_full || as.empty_buffer ) {
                        res = status::DATA_ACQUISITION_ERROR;
                }
                return res;
        }

        void clear_status( status status ) override
        {
                if ( status == status::DATA_ACQUISITION_ERROR ) {
                        ACQUISITION.status().buffer_was_full = false;
                        ACQUISITION.status().empty_buffer    = false;
                }
        }

        uint32_t get_current() const override
        {
                // TODO: well, the hardcoded constant is not ideal
                return ACQUISITION.get_val( 0 );
        }

        void set_current_callback( current_cb_interface& cb ) override
        {
                ACQUISITION.set_detailed_callback( cb );
        }
        current_cb_interface& get_current_callback() const override
        {
                return ACQUISITION.get_detailed_callback();
        }
} CURRENT;

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

void HAL_UART_TxCpltCallback( UART_HandleTypeDef* h )
{
        brd::COMMS.tx_cplt_irq( h );
        brd::DEBUG_COMMS.tx_cplt_irq( h );
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
        // TODO: change the API to return bool
        if ( HAL_Init() != HAL_OK ) {
                fw::stop_exec();
        }
        setup_clk();
}

fw::drv::clock* setup_clock()
{
        auto clk_setup = []( TIM_HandleTypeDef& tim, uint32_t& chan ) {
                __HAL_RCC_TIM2_CLK_ENABLE();
                return setup_clock_timer(
                    tim,
                    chan,
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

acquisition_type* setup_acquisition()
{
        auto acq_setup = []( acquisition_type::handles& h ) {
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
                const bool res = setup_hbridge_timers(
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

com_interface* setup_comms()
{
        auto comms_setup = []( fw::drv::cobs_uart::handles& h ) {
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

com_interface* setup_debug_comms()
{
        auto setup_f = []( fw::drv::cobs_uart::handles& h ) {
                __HAL_RCC_DMA1_CLK_ENABLE();
                __HAL_RCC_DMAMUX1_CLK_ENABLE();
                __HAL_RCC_GPIOA_CLK_ENABLE();
                __HAL_RCC_GPIOB_CLK_ENABLE();
                __HAL_RCC_USART1_CLK_ENABLE();
                const bool res = setup_uart(
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

void start_callback( core_drivers& cdrv )
{
        if ( cdrv.position != nullptr ) {
                // this implies that acquisition is OK
                ACQUISITION.start();
        }
        if ( cdrv.motor != nullptr ) {
                HBRIDGE.start();
        }
        if ( cdrv.comms != nullptr ) {
                cdrv.comms->start();
        }
        if ( cdrv.leds != nullptr ) {
                cdrv.leds->start();
        }
}

microseconds get_clock_time()
{
        return CLOCK.get_us();
}

core_drivers setup_core_drivers()
{
        fw::drv::hbridge* hb     = setup_hbridge();
        acquisition_type* acquis = setup_acquisition();
        fw::drv::leds*    leds   = setup_leds();
        fw::install_stop_callback( leds );

        return core_drivers{
            .clock       = setup_clock(),
            .position    = acquis == nullptr ? nullptr : &POSITION,
            .current     = acquis == nullptr ? nullptr : &CURRENT,
            .vcc         = acquis == nullptr ? nullptr : &VCC,
            .temperature = acquis == nullptr ? nullptr : &TEMPERATURE,
            .period_cb   = acquis,
            .motor       = hb,
            .period      = hb,
            .comms       = setup_comms(),
            .leds        = leds,
            .start_cb    = start_callback,
        };
}

}  // namespace brd
