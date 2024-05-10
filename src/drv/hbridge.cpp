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

void hbridge::set_period_callback( period_cb_iface& cb )
{
        period_cb_ = &cb;
}

period_cb_iface& hbridge::get_period_callback()
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

bool hbridge::is_stopped() const
{
        return tim_ == nullptr;
}

void hbridge::set_invert( bool v )
{
        inverted_ = v;
}

void hbridge::set_power( pwr power )
{
        if ( tim_ == nullptr )
                return;

        using namespace std;

        int32_t mc1_val = timer_max_;
        int32_t mc2_val = timer_max_;

        if ( power == p_zero ) {
        } else if ( power > p_zero )
                mc1_val = ( 1.0F - *power ) * static_cast< float >( timer_max_ );
        else
                mc2_val = ( 1.0F + *power ) * static_cast< float >( timer_max_ );

        if ( inverted_ )
                swap( mc1_val, mc2_val );

        __HAL_TIM_SET_COMPARE( tim_, mc1_channel_, static_cast< uint32_t >( mc1_val ) );
        __HAL_TIM_SET_COMPARE( tim_, mc2_channel_, static_cast< uint32_t >( mc2_val ) );
}

int8_t hbridge::get_direction() const
{
        if ( tim_ == nullptr )
                return 1;

        int8_t v = inverted_ ? -1 : 1;

        return __HAL_TIM_GET_COMPARE( tim_, mc1_channel_ ) <=
                       __HAL_TIM_GET_COMPARE( tim_, mc2_channel_ ) ?
                   v :
                   -v;
}

}  // namespace servio::drv
