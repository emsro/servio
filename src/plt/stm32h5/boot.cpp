#include "setup.hpp"

namespace servio::plt
{

[[noreturn]] void enter_bootloader()
{
        // Disable all interrupts and clear pending state
        __disable_irq();
        for ( int i = 0; i < 8; i++ ) {
                NVIC->ICER[i] = 0xFFFFFFFFU;
                NVIC->ICPR[i] = 0xFFFFFFFFU;
        }
        __DSB();
        __ISB();

        // STM32H5 ROM bootloader at system memory base
        constexpr uint32_t bootloader_base = 0x0FFF0000U;
        uint32_t const*    p               = reinterpret_cast< uint32_t const* >( bootloader_base );
        __set_MSP( p[0] );
        reinterpret_cast< void ( * )() >( p[1] )();
        __builtin_unreachable();
}

}  // namespace servio::plt
