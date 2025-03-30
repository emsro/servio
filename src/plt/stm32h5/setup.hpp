#include "../../cfg/map.hpp"
#include "../../drv/defs.hpp"
#include "../../status.hpp"

#include <memory>
#include <optional>

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
        TIM_TypeDef* timer_instance;
        uint16_t     period;
        IRQn_Type    irq;
        uint32_t     irq_priority;
        uint32_t     mc1_ch;
        drv::pin_cfg mc1_pin;
        uint32_t     mc2_ch;
        drv::pin_cfg mc2_pin;
};

struct leds_gpio_cfg
{
        drv::pin_cfg red;
        drv::pin_cfg blue;
};

struct leds_timer_cfg
{
        TIM_TypeDef* timer_instance;
        uint32_t     yellow_ch;
        drv::pin_cfg yellow_pin;
        uint32_t     green_ch;
        drv::pin_cfg green_pin;
};

struct i2c_cfg
{
        I2C_TypeDef* instance;
        IRQn_Type    ev_irq;
        uint32_t     ev_irq_priority;
        IRQn_Type    er_irq;
        uint32_t     er_irq_priority;
        drv::pin_cfg scl;
        drv::pin_cfg sda;
};

struct temp_calib_coeffs
{
        float cal1;
        float cal2;
};

extern temp_calib_coeffs TEMP_CALIB_COEFFS;

cfg::map get_default_config();

status setup_adc( ADC_HandleTypeDef& adc, DMA_HandleTypeDef& dma, adc_cfg cfg );
void setup_adc_channel( ADC_ChannelConfTypeDef& channel, uint32_t ch_id, opt< drv::pin_cfg > cfg );
status setup_adc_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance );

status setup_dac( DAC_HandleTypeDef& dac, drv::pin_cfg const& cfg );

status setup_uart( UART_HandleTypeDef& uart, DMA_HandleTypeDef& tx_dma, uart_cfg cfg );

status setup_i2c( I2C_HandleTypeDef& i2c, i2c_cfg cfg );

status setup_hbridge_timers( TIM_HandleTypeDef& tim, hb_timer_cfg cfg );

void setup_gpio( drv::pin_cfg const& cfg );

status setup_leds_channel( TIM_HandleTypeDef* tim, uint32_t ch, drv::pin_cfg cfg );

status setup_clk();

status setup_clock_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance, IRQn_Type irq );

status setup_encoder_timer( TIM_HandleTypeDef& tim, TIM_TypeDef* instance, uint32_t period );

status setup_dts( DTS_HandleTypeDef& h, DTS_TypeDef* inst );

}  // namespace servio::plt
