#include "cfg/map.hpp"
#include "fw/board.hpp"
#include "fw/drv/hbridge.hpp"  // tehse are actually a bad sign TODO
#include "fw/drv/leds.hpp"

#include <memory>

namespace plt
{

struct dma_cfg
{
        DMA_Channel_TypeDef* instance;
        IRQn_Type            irq;
        uint32_t             irq_priority;
        uint32_t             request;
        uint32_t             priority;
};

struct pinch_cfg
{
        uint32_t      channel;
        uint32_t      pin;
        GPIO_TypeDef* port;
        uint8_t       alternate = 0x0;
};

struct adc_cfg
{
        ADC_TypeDef* adc_instance;
        uint32_t     adc_irq_priority;
        dma_cfg      dma;
};

struct adc_timer_cfg
{
        TIM_TypeDef* timer_instance;
        uint32_t     channel;
};

struct pin_cfg
{
        uint16_t      pin;
        GPIO_TypeDef* port;
        uint8_t       alternate = 0x0;
};

struct uart_cfg
{
        USART_TypeDef* uart_instance;
        uint32_t       baudrate;
        IRQn_Type      irq;
        uint32_t       irq_priority;
        pin_cfg        rx;
        pin_cfg        tx;
        dma_cfg        tx_dma;
};

struct hb_timer_cfg
{
        TIM_TypeDef* timer_instance;
        uint16_t     period;
        IRQn_Type    irq;
        uint32_t     irq_priority;
        pinch_cfg    mc1;
        pinch_cfg    mc2;
};

struct leds_gpio_cfg
{
        pin_cfg red;
        pin_cfg blue;
};

struct leds_timer_cfg
{
        TIM_TypeDef* timer_instance;
        pinch_cfg    yellow;
        pinch_cfg    green;
};

struct clock_timer_cfg
{
        TIM_TypeDef* timer_instance;
        uint32_t     channel;
};

cfg::map get_default_config();

em::result setup_adc( ADC_HandleTypeDef& adc, DMA_HandleTypeDef& dma, adc_cfg cfg );
void       setup_adc_channel( ADC_ChannelConfTypeDef& channel, pinch_cfg cfg );

em::result setup_adc_timer( TIM_HandleTypeDef& tim, uint32_t& tim_channel, adc_timer_cfg cfg );

em::result setup_uart( UART_HandleTypeDef& uart, DMA_HandleTypeDef& tx_dma, uart_cfg cfg );

em::result setup_hbridge_timers( fw::drv::hbridge::handles&, hb_timer_cfg cfg );

em::result setup_leds_gpio( fw::drv::leds::handles&, leds_gpio_cfg cfg );

em::result setup_leds_timer( fw::drv::leds::handles&, leds_timer_cfg cfg );
em::result setup_clk();

em::result setup_clock_timer( TIM_HandleTypeDef& tim, uint32_t& channel, clock_timer_cfg cfg );

}  // namespace plt
