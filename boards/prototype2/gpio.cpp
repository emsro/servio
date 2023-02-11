#include "fw/drivers/leds.hpp"

namespace brd
{

// MAP:
//  PF0 - Y
//  PF1 - G
//  PB0 - G
//  PB5 - Y
bool setup_leds_gpio( fw::leds::handles& h )
{
        h.red_peripheral  = GPIOF;
        h.red_pin         = GPIO_PIN_0;
        h.blue_peripheral = GPIOF;
        h.blue_pin        = GPIO_PIN_1;

        GPIO_InitTypeDef init;
        init.Pin   = h.red_pin | h.blue_pin;
        init.Mode  = GPIO_MODE_OUTPUT_PP;
        init.Pull  = GPIO_NOPULL;
        init.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init( GPIOF, &init );

        return true;
}

}  // namespace brd
