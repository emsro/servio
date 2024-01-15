#include "base/base.hpp"
#include "base/drv_interfaces.hpp"
#include "platform.hpp"

#include <emlabcpp/experimental/function_view.h>

#pragma once

namespace servio::drv
{

class clock : public base::clk_interface
{
public:
        clock() = default;

        clock( const clock& )            = delete;
        clock( clock&& )                 = delete;
        clock& operator=( const clock& ) = delete;
        clock& operator=( clock&& )      = delete;

        clock* setup( TIM_HandleTypeDef* tim )
        {
                tim_ = tim;
                __HAL_TIM_ENABLE( tim_ );
                return this;
        }

        base::microseconds get_us() override
        {
                return base::microseconds{ __HAL_TIM_GET_COUNTER( tim_ ) };
        }

private:
        TIM_HandleTypeDef* tim_;
};

}  // namespace servio::drv
