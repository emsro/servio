#include "base/base.hpp"
#include "drv/interfaces.hpp"
#include "platform.hpp"

#include <emlabcpp/experimental/function_view.h>

#pragma once

namespace servio::drv
{

class clock : public clk_interface
{
public:
        clock( TIM_HandleTypeDef& tim )
          : tim_( tim )
        {
        }

        clock( const clock& )            = delete;
        clock( clock&& )                 = delete;
        clock& operator=( const clock& ) = delete;
        clock& operator=( clock&& )      = delete;

        base::microseconds get_us() override
        {
                return base::microseconds{ __HAL_TIM_GET_COUNTER( &tim_ ) };
        }

private:
        TIM_HandleTypeDef& tim_;
};

}  // namespace servio::drv
