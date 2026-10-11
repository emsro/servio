#include "hbridge.hpp"

#include <emlabcpp/algorithm.hpp>

namespace servio::drv
{

error_code hbridge::start()
{
        if ( tim_ == nullptr )
                return status::error;
        if ( HAL_TIM_PWM_Start_IT( tim_, mc1_channel_ ) != HAL_OK )
                return status::error;
        if ( HAL_TIM_PWM_Start_IT( tim_, mc2_channel_ ) != HAL_OK )
                return status::error;
        return status::success;
}

error_code hbridge::stop()
{
        if ( tim_ == nullptr )
                return status::error;
        if ( HAL_TIM_PWM_Stop_IT( tim_, mc1_channel_ ) != HAL_OK )
                return status::error;
        if ( HAL_TIM_PWM_Stop_IT( tim_, mc2_channel_ ) != HAL_OK )
                return status::error;
        return status::success;
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

        last_power_ = power;

        auto mc1_val = static_cast< float >( period_ticks_ );
        auto mc2_val = static_cast< float >( period_ticks_ );

        if ( power > 0_pwr ) {
                mc1_val *= 1.0F - *power;
                direction_ = 1;
        } else if ( power < 0_pwr ) {
                mc2_val *= 1.0F + *power;
                direction_ = -1;
        }

        if ( inverted_ )
                swap( mc1_val, mc2_val );

        __HAL_TIM_SET_COMPARE( tim_, mc1_channel_, static_cast< uint32_t >( mc1_val ) );
        __HAL_TIM_SET_COMPARE( tim_, mc2_channel_, static_cast< uint32_t >( mc2_val ) );
}

int8_t hbridge::get_direction() const
{
        return direction_;
}

}  // namespace servio::drv
