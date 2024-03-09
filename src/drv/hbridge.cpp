#include "hbridge.hpp"

#include "emlabcpp/result.h"

#include <emlabcpp/algorithm.h>

namespace servio::drv
{

em::result hbridge::start()
{
        if ( tim_ == nullptr )
                return em::ERROR;
        if ( HAL_TIM_PWM_Start_IT( tim_, mc1_channel_ ) != HAL_OK )
                return em::ERROR;
        if ( HAL_TIM_PWM_Start_IT( tim_, mc2_channel_ ) != HAL_OK )
                return em::ERROR;
        return em::SUCCESS;
}

em::result hbridge::stop()
{
        if ( tim_ == nullptr )
                return em::ERROR;
        if ( HAL_TIM_PWM_Stop_IT( tim_, mc1_channel_ ) != HAL_OK )
                return em::ERROR;
        if ( HAL_TIM_PWM_Stop_IT( tim_, mc2_channel_ ) != HAL_OK )
                return em::ERROR;
        return em::SUCCESS;
}

void hbridge::set_period_callback( period_cb_interface& cb )
{
        period_cb_ = &cb;
}

period_cb_interface& hbridge::get_period_callback()
{
        return *period_cb_;
}

void hbridge::force_stop()
{
        if ( tim_ != nullptr ) {
                __HAL_TIM_SET_COMPARE( tim_, mc1_channel_, 0u );
                __HAL_TIM_SET_COMPARE( tim_, mc2_channel_, 0u );
        }
        tim_ = nullptr;
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
        if ( tim_ == nullptr )
                return;
        __HAL_TIM_SET_COMPARE( tim_, mc1_channel_, static_cast< uint32_t >( mc1_val ) );
        __HAL_TIM_SET_COMPARE( tim_, mc2_channel_, static_cast< uint32_t >( mc2_val ) );
}

int8_t hbridge::get_direction() const
{
        if ( tim_ == nullptr )
                return 1;
        return __HAL_TIM_GET_COMPARE( tim_, mc1_channel_ ) >=
                       __HAL_TIM_GET_COMPARE( tim_, mc2_channel_ ) ?
                   1 :
                   -1;
}

}  // namespace servio::drv
