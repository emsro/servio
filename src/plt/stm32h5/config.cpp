#include "../../cfg/default.hpp"
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

temp_calib_coeffs TEMP_CALIB_COEFFS;

cfg::off_scale calculate_temp_conversion()
{
        auto const cal1 = TEMP_CALIB_COEFFS.cal1;
        auto const cal2 = TEMP_CALIB_COEFFS.cal2;

        float const scale =
            static_cast< float >( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) / ( cal2 - cal1 );

        return { .offset = 30 - cal1 * scale, .scale = scale };
}

consteval cfg::map generate_config()
{
        cfg::map res = cfg::get_default_config();

        float const          r_shunt = 0.043F;
        float const          gain    = 100.F;
        cfg::off_scale const curr_cfg =
            cfg::calculate_current_conversion( 3.3F, 0, 1 << 12, r_shunt, gain );
        res.set_val< cfg::key::CURRENT_CONV_SCALE >( curr_cfg.scale );
        res.set_val< cfg::key::CURRENT_CONV_OFFSET >( curr_cfg.offset );

        return res;
}

cfg::map get_default_config()
{
        cfg::map res = generate_config();

        cfg::off_scale const temp_cfg = calculate_temp_conversion();
        res.set_val< cfg::key::TEMP_CONV_SCALE >( temp_cfg.scale );
        res.set_val< cfg::key::TEMP_CONV_OFFSET >( temp_cfg.offset );

        return res;
}

}  // namespace servio::plt
