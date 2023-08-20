#include "base.hpp"
#include "drv_interfaces.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <stm32g4xx_hal.h>

#pragma once

namespace fw::drv
{

class clock : public clk_interface
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

        clock* setup( auto&& setup_f )
        {
                if ( setup_f( h_.tim, h_.tim_channel ) != em::SUCCESS ) {
                        return nullptr;
                }
                if ( HAL_TIM_OC_Start( &h_.tim, h_.tim_channel ) != HAL_OK ) {
                        return nullptr;
                }
                return this;
        }

        microseconds get_us() override
        {
                return microseconds{ __HAL_TIM_GET_COUNTER( &h_.tim ) };
        }

private:
        handles h_;
};

}  // namespace fw::drv
