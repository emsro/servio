
#include "platform.hpp"

#include "fw/util.hpp"

namespace servio::plt
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

}  // namespace servio::plt
