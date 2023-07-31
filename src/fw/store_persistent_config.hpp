#include "cfg/storage.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/defer.h>

#pragma once

namespace fw
{

inline bool store_persistent_config(
    const em::view< const cfg::page* > pages,
    const cfg::payload&                pld,
    const cfg_map*                     cfg )
{
        std::optional< cfg::page > opt_page = cfg::find_next_page( pages );

        if ( !opt_page ) {
                return false;
        }

        if ( HAL_FLASH_Unlock() != HAL_OK ) {
                fw::stop_exec();
        }
        const em::defer d = [] {
                if ( HAL_FLASH_Lock() != HAL_OK ) {
                        fw::stop_exec();
                }
        };

        const std::byte* start      = opt_page->begin();
        auto             start_addr = reinterpret_cast< uint32_t >( start );

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
                fw::stop_exec();
        }
        if ( erase_err != 0xFFFFFFFF ) {
                fw::stop_exec();
        }

        auto f = [&]( std::size_t offset, uint64_t val ) -> bool {
                const HAL_StatusTypeDef status =
                    HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, start_addr + offset, val );
                if ( status != HAL_OK ) {
                        fw::stop_exec();
                }
                return status == HAL_OK;
        };

        const bool succ = cfg::store( pld, cfg, f );

        return succ;
}

}  // namespace fw
