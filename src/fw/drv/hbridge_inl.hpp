#include "fw/drv/callbacks.hpp"
#include "hbridge.hpp"

#include <emlabcpp/algorithm.h>

namespace fw::drv
{

namespace
{
        empty_period_cb EMPTY_PERIOD_CB;
}

inline hbridge::hbridge()
  : period_cb_( &EMPTY_PERIOD_CB )
{
}

inline bool hbridge::setup( em::function_view< bool( handles& ) > setup_f )
{

        const bool res = setup_f( h_ );

        timer_max_ = h_.timer.Init.Period;

        set_power( 0 );

        return res;
}

inline void hbridge::timer_period_irq( TIM_HandleTypeDef* h )
{
        if ( h != &h_.timer ) {
                return;
        }
        period_cb_->on_period_irq();
}

inline void hbridge::timer_irq()
{
        HAL_TIM_IRQHandler( &h_.timer );
}

inline void hbridge::start()
{
        std::ignore = HAL_TIM_PWM_Start_IT( &h_.timer, h_.mc1_channel );
        std::ignore = HAL_TIM_PWM_Start_IT( &h_.timer, h_.mc2_channel );
}

inline void hbridge::stop()
{
        std::ignore = HAL_TIM_PWM_Stop_IT( &h_.timer, h_.mc1_channel );
        std::ignore = HAL_TIM_PWM_Stop_IT( &h_.timer, h_.mc2_channel );
}

inline void hbridge::set_period_callback( period_cb_interface& cb )
{
        period_cb_ = &cb;
}

inline period_cb_interface& hbridge::get_period_callback()
{
        return *period_cb_;
}

inline void hbridge::set_power( int16_t power )
{

        // TODO: again, casts can potentially produce an error?
        if ( power > 0 ) {
                const auto val = em::map_range< int32_t, int32_t >(
                    power,
                    0,
                    std::numeric_limits< int16_t >::max(),
                    0,
                    static_cast< int32_t >( timer_max_ ) );
                __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc1_channel, static_cast< uint32_t >( val ) );
                __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc2_channel, 0 );
        } else {
                const auto val = em::map_range< int32_t, int32_t >(
                    power,
                    0,
                    std::numeric_limits< int16_t >::lowest(),
                    0,
                    static_cast< int32_t >( timer_max_ ) );
                __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc1_channel, 0 );
                __HAL_TIM_SET_COMPARE( &h_.timer, h_.mc2_channel, static_cast< uint32_t >( val ) );
        }
}

inline int8_t hbridge::get_direction() const
{
        return __HAL_TIM_GET_COMPARE( &h_.timer, h_.mc1_channel ) >=
                       __HAL_TIM_GET_COMPARE( &h_.timer, h_.mc2_channel ) ?
                   1 :
                   -1;
}

}  // namespace fw::drv
