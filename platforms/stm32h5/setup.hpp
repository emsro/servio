#include "cfg/map.hpp"
#include "fw/board.hpp"
#include "fw/drv/defs.hpp"

#include <memory>

namespace servio::plt
{

struct dma_cfg
{
        DMA_Channel_TypeDef* instance;
        uint32_t             request;
        uint32_t             priority;
};


struct adc_cfg
{
        ADC_TypeDef* adc_instance;
        uint32_t     adc_irq_priority;
        uint32_t     adc_trigger;
        dma_cfg      dma;
};

struct uart_cfg
{
        USART_TypeDef*   uart_instance;
        uint32_t         baudrate;
        IRQn_Type        irq;
        uint32_t         irq_priority;
        fw::drv::pin_cfg rx;
        fw::drv::pin_cfg tx;
        dma_cfg          tx_dma;
};

struct hb_timer_cfg
{
        TIM_TypeDef*       timer_instance;
        uint16_t           period;
        IRQn_Type          irq;
        uint32_t           irq_priority;
        fw::drv::pinch_cfg mc1;
        fw::drv::pinch_cfg mc2;
};

struct leds_gpio_cfg
{
        fw::drv::pin_cfg red;
        fw::drv::pin_cfg blue;
};

struct leds_timer_cfg
{
        TIM_TypeDef*       timer_instance;
        fw::drv::pinch_cfg yellow;
        fw::drv::pinch_cfg green;
};

cfg::map get_default_config();

em::result setup_adc( ADC_HandleTypeDef& adc, DMA_HandleTypeDef& dma, adc_cfg cfg );
void       setup_adc_channel( ADC_ChannelConfTypeDef& channel, fw::drv::pinch_cfg cfg );
em::result setup_adc_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance );

em::result setup_uart( UART_HandleTypeDef& uart, DMA_HandleTypeDef& tx_dma, uart_cfg cfg );

em::result setup_hbridge_timers( TIM_HandleTypeDef& tim, hb_timer_cfg cfg );

em::result setup_gpio( const fw::drv::pin_cfg& cfg );

em::result setup_leds_channel( TIM_HandleTypeDef* tim, fw::drv::pinch_cfg cfg );

em::result setup_clk();

em::result setup_clock_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance );

}  // namespace servio::plt
