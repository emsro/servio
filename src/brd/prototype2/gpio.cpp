#include "drv/leds.hpp"
#include "setup.hpp"

namespace servio::brd
{

em::result setup_gpio( const drv::pin_cfg& cfg )
{
        GPIO_InitTypeDef init;
        init.Pin       = cfg.pin;
        init.Mode      = GPIO_MODE_OUTPUT_PP;
        init.Pull      = GPIO_NOPULL;
        init.Speed     = GPIO_SPEED_FREQ_LOW;
        init.Alternate = cfg.alternate;
        HAL_GPIO_Init( cfg.port, &init );

        return em::SUCCESS;
}


}  // namespace servio::brd
