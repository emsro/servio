#include "drv/leds.hpp"
#include "emlabcpp/result.h"
#include "setup.hpp"

namespace servio::plt
{

void setup_gpio( const drv::pin_cfg& cfg )
{
        GPIO_InitTypeDef init;
        init.Pin       = cfg.pin;
        init.Mode      = cfg.mode;
        init.Pull      = cfg.pull;
        init.Speed     = GPIO_SPEED_FREQ_LOW;
        init.Alternate = cfg.alternate;
        HAL_GPIO_Init( cfg.port, &init );
}

}  // namespace servio::plt
