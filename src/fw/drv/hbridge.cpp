#include "hbridge.hpp"

#include <emlabcpp/algorithm.h>

namespace fw::drv
{

void hbridge::start()
{
        std::ignore = HAL_TIM_PWM_Start_IT( &h_.timer, h_.mc1_channel );
        std::ignore = HAL_TIM_PWM_Start_IT( &h_.timer, h_.mc2_channel );
}

void hbridge::stop()
{
        std::ignore = HAL_TIM_PWM_Stop_IT( &h_.timer, h_.mc1_channel );
        std::ignore = HAL_TIM_PWM_Stop_IT( &h_.timer, h_.mc2_channel );
}

void hbridge::set_period_callback( period_cb_interface& cb )
{
        period_cb_ = &cb;
}

period_cb_interface& hbridge::get_period_callback()
{
        return *period_cb_;
}

void hbridge::set_power( int16_t power )
{

        int32_t mc1_val = 0;
        int32_t mc2_val = 0;

        // TODO: again, casts can potentially produce an error?
        if ( power > 0 ) {
                mc1_val = em::map_range< int32_t, int32_t >(
                    power,
                    0,
                    std::numeric_limits< int16_t >::max(),
                    0,
                    static_cast< int32_t >( timer_max_ ) );
        } else {
                mc2_val = em::map_range< int32_t, int32_t >(
                    power,
                    0,
                    std::numeric_limits< int16_t >::lowest(),
                    0,
                    static_cast< int32_t >( timer_max_ ) );
        }
        __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc1_channel, static_cast< uint32_t >( mc1_val ) );
        __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc2_channel, static_cast< uint32_t >( mc2_val ) );
}

int8_t hbridge::get_direction() const
{
        return __HAL_TIM_GET_COMPARE( &h_.timer, h_.mc1_channel ) >=
                       __HAL_TIM_GET_COMPARE( &h_.timer, h_.mc2_channel ) ?
                   1 :
                   -1;
}

}  // namespace fw::drv
