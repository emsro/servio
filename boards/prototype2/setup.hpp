#include "fw/board.hpp"

namespace brd
{

struct dma_cfg
{
        DMA_Channel_TypeDef* instance;
        IRQn_Type            irq;
        uint32_t             irq_priority;
        uint32_t             request;
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
bool setup_adc_timer( fw::acquisition::handles& );
bool setup_duart( fw::debug_comms::handles& );
bool setup_hbridge_timers( fw::hbridge::handles& );
bool setup_iuart( fw::comms::handles& );
bool setup_leds_gpio( fw::leds::handles& );
bool setup_leds_timer( fw::leds::handles& );
void setup_clk();
bool setup_clock_timer( fw::clock::handles& );
void setup_extra();

}  // namespace brd
