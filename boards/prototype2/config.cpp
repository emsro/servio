#include "cfg/default.hpp"
#include "fw/board.hpp"

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

off_scale calculate_temp_conversion()
{
        const auto cal1 = static_cast< float >( *TEMPSENSOR_CAL1_ADDR );
        const auto cal2 = static_cast< float >( *TEMPSENSOR_CAL2_ADDR );

        const float scale =
            static_cast< float >( TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP ) / ( cal2 - cal1 );

        return { .offset = 30 - cal1 * scale, .scale = scale };
}

consteval cfg_map generate_config()
{
        cfg_map res = cfg::get_default_config();

        const float     r_shunt  = 0.043f;
        const float     gain     = 100.f;
        const off_scale curr_cfg = calculate_current_conversion( 3.3f, 0, 1 << 12, r_shunt, gain );
        res.set_val< cfg_key::CURRENT_CONV_SCALE >( curr_cfg.scale );
        res.set_val< cfg_key::CURRENT_CONV_OFFSET >( curr_cfg.offset );

        return res;
}

cfg_map get_config()
{
        cfg_map res = generate_config();

        const off_scale temp_cfg = calculate_temp_conversion();
        res.set_val< cfg_key::TEMP_CONV_SCALE >( temp_cfg.scale );
        res.set_val< cfg_key::TEMP_CONV_OFFSET >( temp_cfg.offset );

        return res;
}
}  // namespace brd

extern "C" {
extern int _config_start;
extern int _config_end;
}

namespace brd
{
em::view< std::byte* > page( uint32_t i )
{
        return em::view_n(
            reinterpret_cast< std::byte* >( &_config_start ) + i * FLASH_PAGE_SIZE,
            FLASH_PAGE_SIZE );
}

std::array< em::view< std::byte* >, 4 > PERSISTENT_BLOCKS{
    page( 0 ),
    page( 1 ),
    page( 2 ),
    page( 3 ) };

em::view< const em::view< std::byte* >* > get_persistent_pages()
{
        return PERSISTENT_BLOCKS;
}

}  // namespace brd
