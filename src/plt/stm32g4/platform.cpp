
#include "platform.hpp"

#include "fw/util.hpp"

namespace servio::plt
{

void cfg_erase( uint32_t start_addr )
{
        FLASH_EraseInitTypeDef erase_cfg{
            .TypeErase = FLASH_TYPEERASE_PAGES,
            .Banks     = FLASH_BANK_1,                                   // eeeeh, study this?
            .Page      = ( start_addr - FLASH_BASE ) / FLASH_PAGE_SIZE,  // this is hardcoded
                                                                         // too mcuh

            .NbPages = 1,
        };

        uint32_t                erase_err;
        const HAL_StatusTypeDef status = HAL_FLASHEx_Erase( &erase_cfg, &erase_err );
        if ( status != HAL_OK ) {
                // TODO: maybe switch to better error handling?
                fw::stop_exec();
        }
}

bool cfg_write( uint32_t addr, uint64_t val )
{
        const HAL_StatusTypeDef status =
            HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, addr, val );
        if ( status != HAL_OK ) {
                fw::stop_exec();
        }
        return status == HAL_OK;
}

}  // namespace servio::plt
