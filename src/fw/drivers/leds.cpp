#include "fw/drivers/leds.hpp"

#include <emlabcpp/algorithm.h>

namespace fw
{

bool leds::setup( em::function_view< bool( handles& ) > setup_f )
{
    return setup_f( h_ );
}

void leds::force_red_led()
{
    HAL_GPIO_WritePin( h_.red_peripheral, h_.red_pin, GPIO_PIN_SET );
}

void leds::start()
{
    HAL_TIM_PWM_Start( &h_.tim, h_.yellow_channel );
    HAL_TIM_PWM_Start( &h_.tim, h_.green_channel );
}

void leds::update( const leds_vals& leds )
{
    if ( !red_forced_ ) {
        HAL_GPIO_WritePin(
            h_.red_peripheral, h_.red_pin, leds.red ? GPIO_PIN_SET : GPIO_PIN_RESET );
    }

    HAL_GPIO_WritePin( h_.blue_peripheral, h_.blue_pin, leds.blue ? GPIO_PIN_SET : GPIO_PIN_RESET );

    uint16_t yellow_val = em::map_range< uint8_t, uint32_t >(
        leds.yellow, 0u, std::numeric_limits< uint8_t >::max(), 0u, h_.tim.Init.Period );
    __HAL_TIM_SET_COMPARE( &h_.tim, h_.yellow_channel, yellow_val );

    uint16_t green_val = em::map_range< uint8_t, uint32_t >(
        leds.green, 0u, std::numeric_limits< uint8_t >::max(), 0u, h_.tim.Init.Period );
    __HAL_TIM_SET_COMPARE( &h_.tim, h_.green_channel, green_val );
}

}  // namespace fw