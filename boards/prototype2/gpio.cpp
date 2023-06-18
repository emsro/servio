#include "fw/drivers/leds.hpp"
#include "setup.hpp"

namespace brd
{

// MAP:
//  PF0 - Y
//  PF1 - G
//  PB0 - G
//  PB5 - Y
bool setup_leds_gpio( fw::leds::handles&, leds_gpio_cfg cfg )
{
        for ( const pin_cfg pc : { cfg.red, cfg.blue } ) {
                GPIO_InitTypeDef init;
                init.Pin       = pc.pin;
                init.Mode      = GPIO_MODE_OUTPUT_PP;
                init.Pull      = GPIO_NOPULL;
                init.Speed     = GPIO_SPEED_FREQ_LOW;
                init.Alternate = pc.alternate;
                HAL_GPIO_Init( pc.port, &init );
        }

        return true;
}

}  // namespace brd
