#include "fw/board.hpp"

#include <emlabcpp/protocol/register_handler.h>
#include <numbers>

namespace brd
{

// TODO: move the formula into library

struct off_scale
{
    float offset;
    float scale;
};

// TODO: move somewhere
struct setpoint
{
    float    angle;
    uint16_t value;
};

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
constexpr off_scale
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

constexpr off_scale calculate_temp_conversion()
{
    auto cal1 = static_cast< float >( *TEMPSENSOR_CAL1_ADDR );
    auto cal2 = static_cast< float >( *TEMPSENSOR_CAL2_ADDR );

    float scale =
        static_cast< float >( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) / ( cal2 - cal1 );

    return { .offset = 30 - cal1 * scale, .scale = scale };
}

using handler = em::protocol::register_handler< cfg_map >;

template < auto ID, typename CB, typename Val >
void kset( CB& cb, const Val& v )
{
    cb( cfg_keyval{ .key = ID, .msg = handler::serialize< ID >( v ) } );
}

void apply_config( em::function_view< void( const cfg_keyval& ) > f )
{
    kset< cfg_key::POSITION_CONV_LOWER_SETPOINT_VALUE >( f, static_cast< uint16_t >( 0u ) );
    kset< cfg_key::POSITION_CONV_LOWER_SETPOINT_ANGLE >( f, 0.f );
    kset< cfg_key::POSITION_CONV_HIGHER_SETPOINT_VALUE >( f, static_cast< uint16_t >( 4096u ) );
    kset< cfg_key::POSITION_CONV_HIGHER_SETPOINT_ANGLE >(
        f, 240.f * std::numbers::pi_v< float > / 180.f );

    static constexpr float r_shunt = 0.043f;
    static constexpr float gain    = 100.f;
    off_scale curr_cfg = calculate_current_conversion( 3.3f, 0, 1 << 12, r_shunt, gain );
    kset< cfg_key::CURRENT_CONV_SCALE >( f, curr_cfg.scale );
    kset< cfg_key::CURRENT_CONV_OFFSET >( f, curr_cfg.offset );

    off_scale temp_cfg = calculate_temp_conversion();
    kset< cfg_key::TEMP_CONV_SCALE >( f, temp_cfg.scale );
    kset< cfg_key::TEMP_CONV_OFFSET >( f, temp_cfg.offset );

    // TODO: fix this
    kset< cfg_key::VOLTAGE_CONV_SCALE >( f, 0.f );
}

}  // namespace brd
