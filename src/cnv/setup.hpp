#pragma once

namespace servio::cnv
{

struct off_scale
{
        float offset;
        float scale;
};

// The formula asserted:
//
//            V_min                      V_max - V_min
// I_shunt = ---------------- + adc_val ----------------------------
//            R_shunt * gain             adc_steps * R_shunt * gain
//
// We need form:
//
// I_shunt = offset + adc_val * scale
//
// That implies:
//
// offset = V_min / ( R_shunt * gain )
// scale = (V_max - V_min) / (adc_steps * R_shunt * gain)
//
constexpr off_scale
calc_current_conversion( float v_max, float v_min, float steps, float r_shunt, float gain )
{
        return {
            .offset = v_min / ( r_shunt * gain ),
            .scale  = ( v_max - v_min ) / ( steps * r_shunt * gain ),
        };
}

}  // namespace servio::cnv
