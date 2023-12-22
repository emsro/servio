#include "cfg/default.hpp"
#include "cfg/math.hpp"
#include "fw/board.hpp"
#include "platform.hpp"

#include <numbers>

namespace plt
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
        const auto cal1 = static_cast< float >( *TEMPSENSOR_CAL1_ADDR );
        const auto cal2 = static_cast< float >( *TEMPSENSOR_CAL2_ADDR );

        const float scale =
            static_cast< float >( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) / ( cal2 - cal1 );

        return { .offset = 30 - cal1 * scale, .scale = scale };
}

consteval cfg::map generate_config()
{
        cfg::map res = cfg::get_default_config();

        const float          r_shunt = 0.043F;
        const float          gain    = 100.F;
        const cfg::off_scale curr_cfg =
            cfg::calculate_current_conversion( 3.3F, 0, 1 << 12, r_shunt, gain );
        res.set_val< cfg::key::CURRENT_CONV_SCALE >( curr_cfg.scale );
        res.set_val< cfg::key::CURRENT_CONV_OFFSET >( curr_cfg.offset );

        return res;
}

cfg::map get_default_config()
{
        cfg::map res = generate_config();

        const cfg::off_scale temp_cfg = calculate_temp_conversion();
        res.set_val< cfg::key::TEMP_CONV_SCALE >( temp_cfg.scale );
        res.set_val< cfg::key::TEMP_CONV_OFFSET >( temp_cfg.offset );

        return res;
}
}  // namespace plt
