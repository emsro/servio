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

struct check_bool
{
};

inline void operator<<( check_bool const&, bool val )
{
        if ( !val )
                stop_exec();
}

}  // namespace servio::fw
