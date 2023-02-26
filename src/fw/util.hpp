#include <chrono>
#include <emlabcpp/algorithm.h>
#include <stm32g4xx_hal.h>

#pragma once

namespace em = emlabcpp;

namespace fw
{

void stop_exec();

inline std::chrono::microseconds ticks_ms()
{
        return std::chrono::microseconds{ HAL_GetTick() * 1000 };
}

template < typename... Components >
void multistart( Components&... comps )
{
        ( comps.start(), ... );
}

struct check_bool
{
};

inline void operator<<( const check_bool&, bool val )
{
        if ( !val ) {
                stop_exec();
        }
}

struct hal_check
{
};

inline void operator<<( hal_check, HAL_StatusTypeDef s )
{
        if ( s != HAL_OK ) {
                stop_exec();
        }
}

}  // namespace fw
