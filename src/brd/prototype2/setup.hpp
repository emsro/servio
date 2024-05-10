#include "drv/defs.hpp"
#include "drv/hbridge.hpp"  // tehse are actually a bad sign TODO
#include "drv/leds.hpp"

namespace servio::brd
{

struct dma_cfg
{
        DMA_Channel_TypeDef* instance;
        IRQn_Type            irq;
        uint32_t             irq_priority;
        uint32_t             request;
        uint32_t             priority;
};

struct adc_cfg
{
        ADC_TypeDef* adc_instance;
        uint32_t     adc_irq_priority;
        dma_cfg      dma;
};

em::result setup_adc( ADC_HandleTypeDef& adc, DMA_HandleTypeDef& dma, adc_cfg cfg );
void       setup_adc_channel(
          ADC_ChannelConfTypeDef&       channel,
          uint32_t                      chann_id,
          std::optional< drv::pin_cfg > cfg );
em::result setup_adc_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance );

struct uart_cfg
{
        USART_TypeDef* uart_instance;
        uint32_t       baudrate;
        IRQn_Type      irq;
        uint32_t       irq_priority;
        drv::pin_cfg   rx;
        drv::pin_cfg   tx;
        dma_cfg        tx_dma;
};

em::result setup_uart( UART_HandleTypeDef& uart, DMA_HandleTypeDef& tx_dma, uart_cfg cfg );

struct hb_timer_cfg
{
        TIM_TypeDef* timer_instance;
        uint16_t     period;
        IRQn_Type    irq;
        uint32_t     irq_priority;
        uint32_t     mc1_chan;
        drv::pin_cfg mc1_pin;
        uint32_t     mc2_chan;
        drv::pin_cfg mc2_pin;
};

em::result setup_hbridge_timers( TIM_HandleTypeDef& tim, hb_timer_cfg cfg );
void       setup_gpio( const drv::pin_cfg& cfg );

struct leds_timer_cfg
{
        TIM_TypeDef* timer_instance;
        uint32_t     yellow_chan;
        drv::pin_cfg yellow;
        uint32_t     green_chan;
        drv::pin_cfg green;
};

em::result setup_leds_channel( TIM_HandleTypeDef& tim, uint32_t chann, drv::pin_cfg cfg );
em::result setup_leds_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance );

void setup_clk();

em::result setup_clock_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance );

}  // namespace servio::brd
