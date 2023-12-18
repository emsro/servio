
#include "platform.hpp"

#include "fw/util.hpp"

namespace plt
{

void cfg_erase( uint32_t start_addr )
{
        // TODO: check this!
        FLASH_EraseInitTypeDef erase_cfg{
            .TypeErase = FLASH_TYPEERASE_SECTORS,
            .Banks     = FLASH_BANK_1,                                     // eeeeh, study this?
            .Sector    = ( start_addr - FLASH_BASE ) / FLASH_SECTOR_SIZE,  // this is hardcoded
                                                                           // too mcuh

            .NbSectors = 1,
        };

        uint32_t                erase_err;
        const HAL_StatusTypeDef status = HAL_FLASHEx_Erase( &erase_cfg, &erase_err );
        if ( status != HAL_OK ) {
                // TODO: maybe switch to better error handling?
                fw::stop_exec();
        }
        if ( erase_err != 0xFFFFFFFF ) {
                fw::stop_exec();
        }
}

bool cfg_write( uint32_t addr, uint64_t val )
{
        // TODO: is the enum correct?
        const HAL_StatusTypeDef status = HAL_FLASH_Program( FLASH_TYPEPROGRAM_OB, addr, val );
        if ( status != HAL_OK ) {
                fw::stop_exec();
        }
        return status == HAL_OK;
}

}  // namespace plt
