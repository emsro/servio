#include "control.hpp"
#include "converter.hpp"
#include "drivers/acquisition.hpp"
#include "drivers/hbridge.hpp"
#include "metrics.hpp"
#include "stm32g4xx_hal.h"

#pragma once

namespace fw
{

struct acquisition_period_callback
{
        acquisition& acq;

        void operator()()
        {
                acq.period_elapsed_irq();
        }
};

struct current_callback
{
        hbridge&         hb;
        control&         ctl;
        const converter& conv;

        void operator()( uint32_t current, std::span< uint16_t > )
        {
                std::chrono::milliseconds now{ HAL_GetTick() };

                float c = conv.convert_current( current, hb.get_direction() );

                ctl.current_irq( now, c );
                hb.set_power( ctl.get_power() );
        }
};

struct position_callback
{
        control&         ctl;
        metrics&         met;
        const converter& conv;

        void operator()( uint32_t position )
        {
                std::chrono::milliseconds now{ HAL_GetTick() };

                float p = conv.convert_position( position );

                met.position_irq( now, p );
                ctl.position_irq( now, met.get_position() );
                ctl.velocity_irq( now, met.get_velocity() );
        }
};

}  // namespace fw
