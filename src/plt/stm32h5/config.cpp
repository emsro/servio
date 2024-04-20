#include "cfg/default.hpp"
#include "cfg/math.hpp"
#include "fw/util.hpp"
#include "platform.hpp"
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
        const auto cal1 = TEMP_CALIB_COEFFS.cal1;
        const auto cal2 = TEMP_CALIB_COEFFS.cal2;

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

void cfg_erase( uint32_t start_addr )
{
        uint32_t flash_offset = start_addr - FLASH_BASE;
        uint32_t bank         = flash_offset < FLASH_BANK_SIZE ? FLASH_BANK_1 : FLASH_BANK_2;
        uint32_t sector =
            ( bank == FLASH_BANK_1 ? flash_offset : ( flash_offset - FLASH_BANK_SIZE ) ) /
            FLASH_SECTOR_SIZE;

        FLASH_EraseInitTypeDef erase_cfg{
            .TypeErase = FLASH_TYPEERASE_SECTORS,
            .Banks     = bank,
            .Sector    = sector,
            .NbSectors = 1,
        };

        uint32_t                erase_err;
        const HAL_StatusTypeDef status = HAL_FLASHEx_Erase( &erase_cfg, &erase_err );
        if ( status != HAL_OK ) {
                // TODO: maybe switch to better error handling?
                fw::stop_exec();
        }
        if ( erase_err != 0xFFFFFFFF )
                fw::stop_exec();
}

bool cfg_write( uint32_t addr, std::span< std::byte > buffer )
{
        static constexpr std::size_t n = 128 / 8;
        using dword                    = std::array< std::byte, n >;

        while ( !buffer.empty() ) {
                std::span< std::byte > tmp = buffer.subspan( 0, std::min( n, buffer.size() ) );

                dword var{};
                std::memcpy( var.data(), tmp.data(), tmp.size() );
                const HAL_StatusTypeDef status = HAL_FLASH_Program(
                    FLASH_TYPEPROGRAM_QUADWORD, addr, std::bit_cast< uint32_t >( var.data() ) );
                if ( status != HAL_OK )
                        fw::stop_exec();

                buffer = buffer.subspan( tmp.size() );
                addr += tmp.size();
        }
        // TODO: well, there is error handling to do
        return true;
}

void cfg_store( uint32_t addr, std::span< std::byte > buffer )
{
        if ( HAL_ICACHE_Disable() != HAL_OK )
                fw::stop_exec();
        if ( HAL_FLASH_Unlock() != HAL_OK )
                fw::stop_exec();
        const em::defer d = [] {
                if ( HAL_ICACHE_Enable() != HAL_OK )
                        fw::stop_exec();
                if ( HAL_FLASH_Lock() != HAL_OK )
                        fw::stop_exec();
        };
        cfg_erase( addr );
        cfg_write( addr, buffer );
}

}  // namespace servio::plt
