#include "base.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <stm32g4xx_hal.h>

#pragma once

namespace fw
{

class clock
{
public:
        struct handles
        {
                TIM_HandleTypeDef tim;
                uint32_t          tim_channel;
        };

        clock() = default;

        clock( const clock& )            = delete;
        clock( clock&& )                 = delete;
        clock& operator=( const clock& ) = delete;
        clock& operator=( clock&& )      = delete;

        bool setup( em::function_view< bool( handles& ) > setup_f )
        {
                bool res = setup_f( h_ );
                HAL_TIM_OC_Start( &h_.tim, h_.tim_channel );
                return res;
        }

        microseconds get_us()
        {
                return microseconds{ __HAL_TIM_GET_COUNTER( &h_.tim ) };
        }

private:
        handles h_;
};

}  // namespace fw
