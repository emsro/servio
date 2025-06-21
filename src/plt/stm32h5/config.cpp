#include "../../cfg/math.hpp"
#include "../../fw/util.hpp"
#include "../platform.hpp"
#include "setup.hpp"

#include <emlabcpp/defer.h>
#include <numbers>

namespace servio::plt
{

// The formula asserted:
//
//                                       cal2_temp - cal1_temp
//  T (°C) = 30 + ( temp_val - cal1 ) * ------------------------
//                                            cal2 - cal1
//
//                        cal2_temp - cal1_temp                 cal2_temp - cal1_temp
//  T (°C) = 30 - cal1 * ------------------------ + temp_val * ------------------------
//                             cal2 - cal1                           cal2 - cal1
//
//
//  We need form:
//
//  T = offset + adc_val * scale
//
//  That implies:
//
//  scale = ( cal2_temp - cal1_temp ) / ( cal2 - cal1 )
//  offset = 30 - cal1 * scale
//
//

cfg::off_scale calculate_temp_conversion()
{
        auto cal1 = *TEMPSENSOR_CAL1_ADDR;
        auto cal2 = *TEMPSENSOR_CAL2_ADDR;

        float const scale =
            static_cast< float >( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) / ( cal2 - cal1 );

        return { .offset = 30 - cal1 * scale, .scale = scale };
}

cfg::map get_default_config()
{
        cfg::map res{};

        cfg::off_scale const temp_cfg = calculate_temp_conversion();
        res.temp_conv_scale           = temp_cfg.scale;
        res.temp_conv_offset          = temp_cfg.offset;

        return res;
}

}  // namespace servio::plt
