#include <algorithm>
#include <cstring>
#include <stm32g4xx_hal.h>

#pragma once

namespace fw
{

void stop_exec();

template < typename... Components >
void multistart( Components&... comps )
{
        ( comps.start(), ... );
}

void copy_string_to( const char* str, std::size_t size, auto& target )
{
        std::memset( target, '\0', sizeof( target ) );
        std::strncpy( target, str, std::min( size, sizeof( target ) - 1 ) );
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
