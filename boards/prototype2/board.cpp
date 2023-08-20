#include "brd.hpp"
#include "core_drivers.hpp"
#include "fw/drv/adc_pooler.hpp"
#include "fw/drv/clock.hpp"
#include "fw/drv/cobs_uart.hpp"
#include "fw/util.hpp"
#include "setup.hpp"

namespace brd
{
enum chan_ids
{
        CURRENT_CHANNEL,
        POSITION_CHANNEL,
        VCC_CHANNEL,
        TEMP_CHANNEL,
};

struct adc_set
{
        // TODO: only some of the adc channels require callback infrastructure

        using id_type = chan_ids;

        fw::drv::detailed_adc_channel< CURRENT_CHANNEL, 128 >  current;
        fw::drv::adc_channel_with_callback< POSITION_CHANNEL > position;
        fw::drv::adc_channel< VCC_CHANNEL >                    vcc;
        fw::drv::adc_channel< TEMP_CHANNEL >                   temp;

        auto tie()
        {
                return std::tie( current, position, vcc, temp );
        }
};

fw::drv::clock                 CLOCK{};
fw::drv::adc_pooler< adc_set > ADC_POOLER{};
fw::drv::cobs_uart             COMMS{};
fw::drv::cobs_uart             DEBUG_COMMS{};
fw::drv::hbridge               HBRIDGE{};
fw::drv::leds                  LEDS;

struct : period_cb_interface
{
        void on_period_irq()
        {
                ADC_POOLER.on_period_irq();
        }
} PERIOD_CB;

struct : vcc_interface
{
        status get_status() const override
        {
                return ADC_POOLER->vcc.get_status();
        }

        uint32_t get_vcc() const override
        {
                return ADC_POOLER->vcc.last_value;
        }
} VCC;

struct : temperature_interface
{
        status get_status() const override
        {
                return ADC_POOLER->temp.get_status();
        }

        uint32_t get_temperature() const override
        {
                return ADC_POOLER->temp.last_value;
        }
} TEMPERATURE;

struct : position_interface
{
        status get_status() const override
        {
                return ADC_POOLER->position.get_status();
        }

        uint32_t get_position() const override
        {
                return ADC_POOLER->position.last_value;
        }

        void set_position_callback( position_cb_interface& cb ) override
        {
                ADC_POOLER->position.callback = &cb;
        }
        position_cb_interface& get_position_callback() const override
        {
                return *ADC_POOLER->position.callback;
        }
} POSITION;

struct : current_interface
{
        status get_status() const override
        {
                return ADC_POOLER->current.get_status();
        }

        void clear_status() override
        {
                ADC_POOLER->current.clear_status();
        }

        uint32_t get_current() const override
        {
                return ADC_POOLER->current.last_value;
        }

        void set_current_callback( current_cb_interface& cb ) override
        {
                ADC_POOLER->current.callback = &cb;
        }
        current_cb_interface& get_current_callback() const override
        {
                return *ADC_POOLER->current.callback;
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

[[gnu::flatten]] void DMA1_Channel1_IRQHandler()
{
        brd::ADC_POOLER.dma_irq();
}
[[gnu::flatten]] void ADC1_2_IRQHandler()
{
        brd::ADC_POOLER.adc_irq();
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
[[gnu::flatten]] void HAL_UART_RxCpltCallback( UART_HandleTypeDef* h )
{
        brd::COMMS.rx_cplt_irq( h );
        brd::DEBUG_COMMS.rx_cplt_irq( h );
}
[[gnu::flatten]] void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* h )
{
        brd::ADC_POOLER.adc_conv_cplt_irq( h );
}
[[gnu::flatten]] void HAL_ADC_ErrorCallback( ADC_HandleTypeDef* h )
{
        brd::ADC_POOLER.adc_error_irq( h );
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

em::result clock_setup( TIM_HandleTypeDef& tim, uint32_t& chan )
{
        __HAL_RCC_TIM2_CLK_ENABLE();
        return setup_clock_timer(
            tim,
            chan,
            clock_timer_cfg{
                .timer_instance = TIM2,
                .channel        = TIM_CHANNEL_1,
            } );
}

auto ADC_SEQUENCE = std::array{
    CURRENT_CHANNEL,
    POSITION_CHANNEL,
    CURRENT_CHANNEL,
    VCC_CHANNEL,
    CURRENT_CHANNEL,
    POSITION_CHANNEL,
    CURRENT_CHANNEL,
    TEMP_CHANNEL,
};

em::result adc_pooler_setup(
    ADC_HandleTypeDef& adc,
    DMA_HandleTypeDef& dma,
    TIM_HandleTypeDef& tim,
    uint32_t&          tim_channel )
{
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM4_CLK_ENABLE();
        setup_adc_channel(
            ADC_POOLER->current.chconf,
            pinch_cfg{
                .channel = ADC_CHANNEL_4,
                .pin     = GPIO_PIN_3,
                .port    = GPIOA,
            } );
        setup_adc_channel(
            ADC_POOLER->position.chconf,
            pinch_cfg{
                .channel = ADC_CHANNEL_1,
                .pin     = GPIO_PIN_0,
                .port    = GPIOA,
            } );
        setup_adc_channel(
            ADC_POOLER->vcc.chconf,
            pinch_cfg{
                .channel = ADC_CHANNEL_2,
                .pin     = GPIO_PIN_1,
                .port    = GPIOA,
            } );
        setup_adc_channel(
            ADC_POOLER->temp.chconf,
            pinch_cfg{
                .channel = ADC_CHANNEL_TEMPSENSOR_ADC1,
                .pin     = 0,
                .port    = nullptr,
            } );
        return em::worst_of(
            setup_adc(
                adc,
                dma,
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
            setup_adc_timer(
                tim,
                tim_channel,
                adc_timer_cfg{
                    .timer_instance = TIM4,
                    .channel        = TIM_CHANNEL_1,
                } ) );
}

em::result hbridge_setup( fw::drv::hbridge::handles& h )
{
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        return setup_hbridge_timers(
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
}

em::result comms_setup( fw::drv::cobs_uart::handles& h )
{
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
}

em::result debug_comms_setup( fw::drv::cobs_uart::handles& h )
{
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();
        return setup_uart(
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
}

com_interface* setup_debug_comms()
{
        return DEBUG_COMMS.setup( debug_comms_setup );
}

em::result leds_setup( fw::drv::leds::handles& h )
{
        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM3_CLK_ENABLE();
        return em::worst_of(
            setup_leds_gpio(
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
                } ),
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
                } ) );
}

em::result start_callback( core_drivers& cdrv )
{

        if ( cdrv.position != nullptr ) {
                // this implies that adc_poolerition is OK
                if ( ADC_POOLER.start() != em::SUCCESS ) {
                        return em::ERROR;
                }
        }
        if ( cdrv.motor != nullptr ) {
                HBRIDGE.start();
        }
        if ( cdrv.comms != nullptr ) {
                std::ignore = cdrv.comms->start();
        }
        if ( cdrv.leds != nullptr ) {
                LEDS.start();
        }

        return em::SUCCESS;
}

microseconds get_clock_time()
{
        return CLOCK.get_us();
}

core_drivers setup_core_drivers()
{
        fw::drv::hbridge*               hb = HBRIDGE.setup( hbridge_setup );
        fw::drv::adc_pooler< adc_set >* adc_pooler =
            ADC_POOLER.setup( ADC_SEQUENCE, adc_pooler_setup );
        fw::drv::leds* leds = LEDS.setup( leds_setup );
        fw::install_stop_callback( leds );

        return core_drivers{
            .clock       = CLOCK.setup( clock_setup ),
            .position    = adc_pooler == nullptr ? nullptr : &POSITION,
            .current     = adc_pooler == nullptr ? nullptr : &CURRENT,
            .vcc         = adc_pooler == nullptr ? nullptr : &VCC,
            .temperature = adc_pooler == nullptr ? nullptr : &TEMPERATURE,
            .period_cb   = adc_pooler == nullptr ? nullptr : &PERIOD_CB,
            .motor       = hb,
            .period      = hb,
            .comms       = COMMS.setup( comms_setup ),
            .leds        = leds,
            .start_cb    = start_callback,
        };
}

}  // namespace brd
