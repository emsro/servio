#include "hbridge.hpp"

namespace fw
{

bool hbridge::setup( em::function_view< bool( handles& ) > setup_f, const config& cfg )
{

    bool res = setup_f( h_ );

    timer_max_ = h_.timer.Init.Period;

    set_power( 0 );

    return res;
}

void hbridge::timer_period_irq()
{
    if ( period_cb_ ) {
        period_cb_();
    }
}

void hbridge::timer_irq()
{
    HAL_TIM_IRQHandler( &h_.timer );
}

void hbridge::start()
{
    HAL_TIM_PWM_Start_IT( &h_.timer, h_.mc1_channel );
    HAL_TIM_PWM_Start_IT( &h_.timer, h_.mc2_channel );
}

void hbridge::set_period_callback( period_callback cb )
{
    period_cb_ = std::move( cb );
}

const hbridge::period_callback& hbridge::get_period_callback()
{
    return period_cb_;
}

void hbridge::set_power( int16_t power )
{

    // TODO: again, casts can potentially produce an error?
    if ( power > 0 ) {
        int32_t val = em::map_range< int32_t, int32_t >(
            power,
            0,
            std::numeric_limits< int16_t >::max(),
            0,
            static_cast< int32_t >( timer_max_ ) );
        __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc1_channel, static_cast< uint32_t >( val ) );
        __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc2_channel, 0 );
    } else {
        int32_t val = em::map_range< int32_t, int32_t >(
            power,
            0,
            std::numeric_limits< int16_t >::lowest(),
            0,
            static_cast< int32_t >( timer_max_ ) );
        __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc1_channel, 0 );
        __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc2_channel, static_cast< uint32_t >( val ) );
    }
}

int8_t hbridge::get_direction() const
{
    return __HAL_TIM_GET_COMPARE( &h_.timer, h_.mc1_channel ) >=
                   __HAL_TIM_GET_COMPARE( &h_.timer, h_.mc2_channel ) ?
               1 :
               -1;
}

}  // namespace fw
