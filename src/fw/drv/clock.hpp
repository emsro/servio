#include "base.hpp"
#include "drv_interfaces.hpp"
#include "platform.hpp"

#include <emlabcpp/experimental/function_view.h>

#pragma once

namespace fw::drv
{

class clock : public clk_interface
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

        microseconds get_us() override
        {
                return microseconds{ __HAL_TIM_GET_COUNTER( tim_ ) };
        }

private:
        TIM_HandleTypeDef* tim_;
};

}  // namespace fw::drv
