#include "../../drv/leds.hpp"
#include "setup.hpp"

#include <emlabcpp/result.h>

namespace servio::plt
{

void setup_gpio( drv::pin_cfg const& cfg )
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
