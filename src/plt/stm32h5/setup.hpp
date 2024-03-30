#include "cfg/map.hpp"
#include "drv/defs.hpp"

#include <emlabcpp/result.h>
#include <memory>

namespace em = emlabcpp;

namespace servio::plt
{

struct dma_cfg
{
        IRQn_Type            irq;
        uint32_t             irq_priority;
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
        USART_TypeDef* uart_instance;
        uint32_t       baudrate;
        IRQn_Type      irq;
        uint32_t       irq_priority;
        drv::pin_cfg   rx;
        drv::pin_cfg   tx;
        dma_cfg        tx_dma;
};

struct hb_timer_cfg
{
        TIM_TypeDef*   timer_instance;
        uint16_t       period;
        IRQn_Type      irq;
        uint32_t       irq_priority;
        drv::pinch_cfg mc1;
        drv::pinch_cfg mc2;
};

struct leds_gpio_cfg
{
        drv::pin_cfg red;
        drv::pin_cfg blue;
};

struct leds_timer_cfg
{
        TIM_TypeDef*   timer_instance;
        drv::pinch_cfg yellow;
        drv::pinch_cfg green;
};

struct temp_calib_coeffs
{
        float cal1;
        float cal2;
};

extern temp_calib_coeffs TEMP_CALIB_COEFFS;

cfg::map get_default_config();

em::result setup_adc( ADC_HandleTypeDef& adc, DMA_HandleTypeDef& dma, adc_cfg cfg );
void       setup_adc_channel( ADC_ChannelConfTypeDef& channel, drv::pinch_cfg cfg );
em::result setup_adc_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance );

em::result setup_dac( DAC_HandleTypeDef& dac, const drv::pin_cfg& cfg );

em::result setup_uart( UART_HandleTypeDef& uart, DMA_HandleTypeDef& tx_dma, uart_cfg cfg );

em::result setup_hbridge_timers( TIM_HandleTypeDef& tim, hb_timer_cfg cfg );

em::result setup_gpio( const drv::pin_cfg& cfg );

em::result setup_leds_channel( TIM_HandleTypeDef* tim, drv::pinch_cfg cfg );

em::result setup_clk();

em::result setup_clock_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance, IRQn_Type irq );

}  // namespace servio::plt
