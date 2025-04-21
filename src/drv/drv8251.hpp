
#pragma once

#include "../cnv/setup.hpp"

namespace servio::drv::drv8251
{

// XXX: duplicated and actually property of our hbridge
consteval cnv::off_scale get_curr_coeff()
{
        // mirror scale: 1575 uA/A
        // resistor: 1k
        constexpr float gain = 1'575.F / 1'000'000.F;
        return cnv::calc_current_conversion( 3.3F, 0.0F, 1 << 12, 1'000.F, gain );
}

}  // namespace servio::drv::drv8251
