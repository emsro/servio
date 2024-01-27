#include "drv/leds.hpp"

#include "fw/util.hpp"

#include <emlabcpp/algorithm.h>

namespace servio::drv
{

void leds::force_red_led()
{
        HAL_GPIO_WritePin( red_.port, red_.pin, GPIO_PIN_SET );
}

em::result leds::start()
{
        if ( HAL_TIM_PWM_Start( tim_, yellow_.channel ) != HAL_OK )
                return em::ERROR;
        if ( HAL_TIM_PWM_Start( tim_, green_.channel ) != HAL_OK )
                return em::ERROR;
        return em::SUCCESS;
}

void leds::update( const base::leds_vals& leds )
{

        if ( !red_forced_ )
                HAL_GPIO_WritePin( red_.port, red_.pin, leds.red ? GPIO_PIN_SET : GPIO_PIN_RESET );

        HAL_GPIO_WritePin( blue_.port, blue_.pin, leds.blue ? GPIO_PIN_SET : GPIO_PIN_RESET );

        static constexpr uint8_t u_max = std::numeric_limits< uint8_t >::max();
        const auto               per   = static_cast< uint16_t >( tim_->Init.Period );

        const uint16_t yellow_val =
            em::map_range< uint8_t, uint16_t >( leds.yellow, 0U, u_max, 0U, per );
        __HAL_TIM_SET_COMPARE( tim_, yellow_.channel, yellow_val );

        const uint16_t green_val =
            em::map_range< uint8_t, uint16_t >( leds.green, 0U, u_max, 0U, per );
        __HAL_TIM_SET_COMPARE( tim_, green_.channel, green_val );
}

}  // namespace servio::drv
