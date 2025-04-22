#include "./leds.hpp"

#include "../fw/util.hpp"

#include <emlabcpp/algorithm.h>

namespace servio::drv
{

void leds::force_red_led()
{
        HAL_GPIO_WritePin( red_.port, red_.pin, GPIO_PIN_SET );
}

void leds::update( leds_vals const& leds )
{

        if ( !red_forced_ )
                HAL_GPIO_WritePin( red_.port, red_.pin, leds.red ? GPIO_PIN_SET : GPIO_PIN_RESET );

        HAL_GPIO_WritePin( blue_.port, blue_.pin, leds.blue ? GPIO_PIN_SET : GPIO_PIN_RESET );
        HAL_GPIO_WritePin( green_.port, green_.pin, leds.green ? GPIO_PIN_SET : GPIO_PIN_RESET );
}

}  // namespace servio::drv
