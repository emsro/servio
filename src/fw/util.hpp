#include <emlabcpp/algorithm.h>
#include <stm32g4xx_hal.h>
#include <chrono>

#pragma once

namespace em = emlabcpp;

namespace fw
{

void stop_exec();

inline std::chrono::milliseconds ticks_ms()
{
    return std::chrono::milliseconds{ HAL_GetTick() };
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
