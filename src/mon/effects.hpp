#pragma once

#include "base/base.hpp"

namespace servio::mon
{

using namespace base::literals;

struct blinker
{
        static constexpr std::size_t        n     = 5;
        static constexpr base::microseconds step  = 200_ms;
        static constexpr base::microseconds cycle = n * 2 * step;

        std::bitset< n > state;

        bool update( base::microseconds now );

private:
        bool output;

        std::size_t i = 0;

        base::microseconds next_e;
};

float sin_approx( float x );

struct pulser
{
        float val;
        float intensity = 1.F;

        uint8_t update();
};

}  // namespace servio::mon
