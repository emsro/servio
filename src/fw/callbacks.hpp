#include "control.hpp"
#include "drivers/acquisition.hpp"
#include "drivers/hbridge.hpp"
#include "metrics.hpp"
#include "stm32g4xx_hal.h"

namespace fw
{

struct acquisition_period_callback
{
        acquisition& acq;
        hbridge&     hb;

        void operator()()
        {
                acq.period_elapsed_irq( hb.get_direction() );
        }
};

struct current_callback
{
        hbridge& hb;
        control& ctl;

        void operator()( float current )
        {
                std::chrono::milliseconds now{ HAL_GetTick() };
                ctl.current_irq( now, current );
                hb.set_power( ctl.get_power() );
        }
};

struct position_callback
{
        control& ctl;
        metrics& met;

        void operator()( float position )
        {
                std::chrono::milliseconds now{ HAL_GetTick() };

                met.position_irq( now, position );
                ctl.position_irq( now, met.get_position() );
                ctl.velocity_irq( now, met.get_velocity() );
        }
};

}  // namespace fw
