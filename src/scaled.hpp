
#include <emlabcpp/quantity.h>

#pragma once

namespace em = emlabcpp;

template < typename T, int8_t BitOffset >
class scaled : public em::quantity< scaled< T, BitOffset >, T >
{
    static constexpr int32_t multiplicator = 1 << BitOffset;

public:
    using em::quantity< scaled< T, BitOffset >, T >::quantity;

    static scaled from_float( float val )
    {
        return scaled{ val * multiplicator };
    }

    float as_float()
    {
        return static_cast< float >( *this ) / multiplicator;
    }
};
