#include "fw/drv/leds.hpp"

#include "fw/util.hpp"

#include <emlabcpp/algorithm.h>

namespace fw::drv
{

void leds::force_red_led()
{
        HAL_GPIO_WritePin( h_.red_peripheral, h_.red_pin, GPIO_PIN_SET );
}

void leds::start()
{
        std::ignore = HAL_TIM_PWM_Start( &h_.tim, h_.yellow_channel );
        std::ignore = HAL_TIM_PWM_Start( &h_.tim, h_.green_channel );
}

void leds::update( const leds_vals& leds )
{

        if ( !red_forced_ ) {
                HAL_GPIO_WritePin(
                    h_.red_peripheral, h_.red_pin, leds.red ? GPIO_PIN_SET : GPIO_PIN_RESET );
        }

        HAL_GPIO_WritePin(
            h_.blue_peripheral, h_.blue_pin, leds.blue ? GPIO_PIN_SET : GPIO_PIN_RESET );

        static constexpr uint8_t u_max = std::numeric_limits< uint8_t >::max();
        const auto               per   = static_cast< uint16_t >( h_.tim.Init.Period );

        const uint16_t yellow_val =
            em::map_range< uint8_t, uint16_t >( leds.yellow, 0U, u_max, 0U, per );
        __HAL_TIM_SET_COMPARE( &h_.tim, h_.yellow_channel, yellow_val );

        const uint16_t green_val =
            em::map_range< uint8_t, uint16_t >( leds.green, 0U, u_max, 0U, per );
        __HAL_TIM_SET_COMPARE( &h_.tim, h_.green_channel, green_val );
}

}  // namespace fw::drv
