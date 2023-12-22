#include "fw/drv/leds.hpp"
#include "setup.hpp"

namespace plt
{

// MAP:
//  PF0 - Y
//  PF1 - G
//  PB0 - G
//  PB5 - Y
em::result setup_leds_gpio( fw::drv::leds::handles& h, leds_gpio_cfg cfg )
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

        h.red_peripheral = cfg.red.port;
        h.red_pin        = cfg.red.pin;

        h.blue_peripheral = cfg.blue.port;
        h.blue_pin        = cfg.blue.pin;

        return em::SUCCESS;
}

}  // namespace plt
