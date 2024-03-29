#include "base/base.hpp"
#include "drv/interfaces.hpp"
#include "platform.hpp"

#include <emlabcpp/experimental/function_view.h>

#pragma once

namespace servio::drv
{

class clock : public clk_iface
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

        void timer_period_irq( TIM_HandleTypeDef* h )
        {
                if ( h == &tim_ )
                        irq_count_ += 1;
        }

        base::microseconds get_us() override
        {
                uint64_t us;
                uint64_t ic;
                do {
                        ic = irq_count_;
                        us = __HAL_TIM_GET_COUNTER( &tim_ );
                } while ( ic != irq_count_ || ( us & uif_mask ) != 0 );

                uint64_t val = ic * tim_.Init.Period + us;
                return base::microseconds{ val };
        }

private:
        static constexpr uint64_t uif_mask   = 1U << 31;
        volatile uint64_t         irq_count_ = 0;
        TIM_HandleTypeDef&        tim_;
};

}  // namespace servio::drv
