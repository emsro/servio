#include "fw/board.hpp"

namespace brd
{

struct dma_cfg
{
        DMA_Channel_TypeDef* instance;
        IRQn_Type            irq;
        uint32_t             irq_priority;
        uint32_t             request;
        uint32_t             priority;
};

struct adc_pch
{
        uint32_t      channel;
        uint32_t      pin;
        GPIO_TypeDef* port;
};

struct adc_cfg
{
        ADC_TypeDef* adc_instance;
        uint32_t     adc_irq_priority;
        dma_cfg      dma;
        adc_pch      current;
        adc_pch      position;
        adc_pch      vcc;
        adc_pch      temp;
};

bool setup_adc( fw::acquisition::handles&, adc_cfg cfg );

struct adc_timer_cfg
{
        TIM_TypeDef* timer_instance;
        uint32_t     channel;
};

bool setup_adc_timer( fw::acquisition::handles&, adc_timer_cfg cfg );

struct pin_cfg
{
        uint32_t      pin;
        GPIO_TypeDef* port;
        uint8_t       alternate = 0x0;
};

struct duart_cfg
{
        USART_TypeDef* uart_instance;
        uint32_t       baudrate;
        IRQn_Type      irq;
        uint32_t       irq_priority;
        pin_cfg        rx;
        dma_cfg        rx_dma;
        pin_cfg        tx;
        dma_cfg        tx_dma;
};

bool setup_duart( fw::debug_comms::handles&, duart_cfg cfg );
bool setup_hbridge_timers( fw::hbridge::handles& );
bool setup_iuart( fw::comms::handles& );
bool setup_leds_gpio( fw::leds::handles& );
bool setup_leds_timer( fw::leds::handles& );
void setup_clk();
bool setup_clock_timer( fw::clock::handles& );
void setup_extra();

}  // namespace brd
