#include "fw/board.hpp"

#include <numbers>

namespace fw
{

// TODO: move the formula into library

// The formula asserted:
//
//           V_min             V_max - V_min
// I_shunt = ------- + adc_val ---------------------------
//           R_shunt           adc_steps * R_shunt * gain
//
// We need form:
//
// I_shunt = offset + adc_val * scale
//
// That implies:
//
// offset = V_min / R_shunt
// scale = (V_max - V_min) / (adc_steps * R_shunt * gain)
//
constexpr current_converter_config
calculate_current_conversion( float v_max, float v_min, float steps, float r_shunt, float gain )
{
    return {
        .offset = v_min / r_shunt,                                //
        .scale  = ( v_max - v_min ) / ( steps * r_shunt * gain )  //
    };
}

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

constexpr temperature_converter_config calculate_temp_conversion()
{
    auto cal1 = static_cast< float >( *TEMPSENSOR_CAL1_ADDR );
    auto cal2 = static_cast< float >( *TEMPSENSOR_CAL2_ADDR );

    float scale =
        static_cast< float >( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) / ( cal2 - cal1 );

    return { .scale = scale, .offset = 30 - cal1 * scale };
}

void setup_config( config& cfg )
{

    static constexpr float r_shunt = 0.043f;
    static constexpr float gain    = 100.f;
    cfg.current_conv = calculate_current_conversion( 3.3f, 0, 1 << 12, r_shunt, gain );

    // TODO: this is not really _board_ config
    cfg.position_conv = position_converter_config{
        .lower_setpoint  = { .value = 0, .angle = 0.f },
        .higher_setpoint = {
            .value = 4096, .angle = 240.f * std::numbers::pi_v< float > / 180.f } };

    cfg.temp_conv = calculate_temp_conversion();

    cfg.volt_conv = voltage_converter_config{ .scale = 0.f };

    cfg.reversed = false;
}
}  // namespace fw
