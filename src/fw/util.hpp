#include <algorithm>
#include <cstring>

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

}  // namespace fw
