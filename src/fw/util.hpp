#include <algorithm>
#include <cstring>
#include <string_view>

#pragma once

namespace servio::fw
{

void stop_exec();

template < typename... Components >
void multistart( Components&... comps )
{
        ( comps.start(), ... );
}

std::string_view copy_string_to( auto& src, auto& target )
{
        std::memset( target.begin(), '\0', std::size( target ) );
        std::size_t n = std::min( std::size( src ), std::size( target ) - 1 );
        std::strncpy( target.begin(), std::data( src ), n );
        return { target.begin(), n };
}

struct check_bool
{
};

inline void operator<<( check_bool const&, bool val )
{
        if ( !val )
                stop_exec();
}

}  // namespace servio::fw
