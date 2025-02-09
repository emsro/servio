#include "./flash_cfg.hpp"

#include "../fw/util.hpp"

#include <emlabcpp/defer.h>

namespace servio::drv
{

namespace
{

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
        HAL_StatusTypeDef const status = HAL_FLASHEx_Erase( &erase_cfg, &erase_err );
        if ( status != HAL_OK ) {
                // TODO: maybe switch to better error handling?
                fw::stop_exec();
        }
        if ( erase_err != 0xFFFFFFFF )
                fw::stop_exec();
}

bool cfg_write( uint32_t addr, std::span< std::byte const > buffer )
{
        static constexpr std::size_t n = 128 / 8;
        using dword                    = std::array< std::byte, n >;

        while ( !buffer.empty() ) {
                std::span< std::byte const > tmp =
                    buffer.subspan( 0, std::min( n, buffer.size() ) );

                dword var{};
                std::memcpy( var.data(), tmp.data(), tmp.size() );
                HAL_StatusTypeDef const status = HAL_FLASH_Program(
                    FLASH_TYPEPROGRAM_QUADWORD, addr, std::bit_cast< uint32_t >( var.data() ) );
                if ( status != HAL_OK )
                        fw::stop_exec();

                buffer = buffer.subspan( tmp.size() );
                addr += tmp.size();
        }
        // TODO: well, there is error handling to do
        return true;
}
}  // namespace

em::result flash_storage::store_page( std::span< std::byte const > data )
{
        cfg::page const* page = cfg::find_next_page( _pages );

        if ( page == nullptr )
                return em::ERROR;

        std::byte const* start      = page->begin();
        auto             start_addr = std::bit_cast< uint32_t >( start );

        if ( HAL_ICACHE_Disable() != HAL_OK )
                fw::stop_exec();
        if ( HAL_FLASH_Unlock() != HAL_OK )
                fw::stop_exec();

        em::defer const d = [] {
                if ( HAL_ICACHE_Enable() != HAL_OK )
                        fw::stop_exec();
                if ( HAL_FLASH_Lock() != HAL_OK )
                        fw::stop_exec();
        };
        cfg_erase( start_addr );
        if ( cfg_write( start_addr, data ) )
                return em::SUCCESS;
        return em::ERROR;
}

em::outcome flash_storage::load_page( std::span< std::byte > data )
{
        cfg::page const* last_page = cfg::find_latest_page( _pages );
        if ( last_page == nullptr )
                return em::FAILURE;
        if ( data.size() >= last_page->size() )
                return em::ERROR;
        std::memcpy( data.data(), last_page->begin(), data.size() );
        return em::SUCCESS;
}

}  // namespace servio::drv
