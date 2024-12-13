#include "./leds.hpp"

#include "../fw/util.hpp"

#include <emlabcpp/algorithm.h>

namespace servio::drv
{

void leds::force_red_led()
{
        HAL_GPIO_WritePin( red_.port, red_.pin, GPIO_PIN_SET );
}

em::result leds::start()
{
        if ( tim_ == nullptr )
                return em::ERROR;
        if ( HAL_TIM_PWM_Start( tim_, y_chan_ ) != HAL_OK )
                return em::ERROR;
        if ( HAL_TIM_PWM_Start( tim_, g_chan_ ) != HAL_OK )
                return em::ERROR;
        return em::SUCCESS;
}

void leds::update( leds_vals const& leds )
{

        if ( !red_forced_ )
                HAL_GPIO_WritePin( red_.port, red_.pin, leds.red ? GPIO_PIN_SET : GPIO_PIN_RESET );

        HAL_GPIO_WritePin( blue_.port, blue_.pin, leds.blue ? GPIO_PIN_SET : GPIO_PIN_RESET );

        static constexpr uint8_t u_max = std::numeric_limits< uint8_t >::max();
        auto const               per   = static_cast< uint16_t >( tim_->Init.Period );

        uint16_t const yellow_val =
            em::map_range< uint8_t, uint16_t >( leds.yellow, 0U, u_max, 0U, per );
        __HAL_TIM_SET_COMPARE( tim_, y_chan_, yellow_val );

        uint16_t const green_val =
            em::map_range< uint8_t, uint16_t >( leds.green, 0U, u_max, 0U, per );
        __HAL_TIM_SET_COMPARE( tim_, g_chan_, green_val );
}

}  // namespace servio::drv
