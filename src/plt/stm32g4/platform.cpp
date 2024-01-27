
#include "platform.hpp"

#include "fw/util.hpp"

#include <bit>
#include <cstring>
#include <emlabcpp/algorithm.h>

namespace servio::plt
{

namespace em = emlabcpp;

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

bool cfg_write( uint32_t addr, std::span< std::byte > buffer )
{
        using dword = uint64_t;

        while ( !buffer.empty() ) {
                std::span< std::byte > tmp =
                    buffer.first( std::min( sizeof( dword ), buffer.size() ) );
                dword var = 0;
                std::memcpy( &var, tmp.data(), tmp.size() );
                const HAL_StatusTypeDef status =
                    HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, addr, var );
                if ( status != HAL_OK )
                        fw::stop_exec();

                buffer = buffer.subspan( tmp.size() );
                addr += tmp.size();
        }
        // TODO: well, there is error handling to do
        return true;
}

}  // namespace servio::plt
