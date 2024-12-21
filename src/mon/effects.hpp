#pragma once

#include "../base/base.hpp"

namespace servio::mon
{

struct blinker
{
        static constexpr std::size_t  n     = 5;
        static constexpr microseconds step  = 200_ms;
        static constexpr microseconds cycle = n * 2 * step;

        std::bitset< n > state;

        bool update( microseconds now );

private:
        bool output = false;

        std::size_t i = 0;

        microseconds next_e;
};

float sin_approx( float x );

struct pulser
{
        float val;
        float intensity = 1.F;

        uint8_t update();
};

}  // namespace servio::mon
