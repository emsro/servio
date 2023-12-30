#include "fw/util.hpp"
#include "platform.hpp"

// NOLINTBEGIN

extern "C" void SysTick_Handler()
{
        HAL_IncTick();
}

extern "C" void NMI_Handler( void )
{
        while ( true ) {
                servio::fw::stop_exec();
        }
}

extern "C" void HardFault_Handler( void )
{
        while ( true ) {
                servio::fw::stop_exec();
        }
}

extern "C" void MemManage_Handler( void )
{
        while ( true ) {
                servio::fw::stop_exec();
        }
}

extern "C" void BusFault_Handler( void )
{
        while ( true ) {
                servio::fw::stop_exec();
        }
}

extern "C" void UsageFault_Handler( void )
{
        while ( true ) {
                servio::fw::stop_exec();
        }
}

extern "C" void SVC_Handler( void )
{
}

extern "C" void DebugMon_Handler( void )
{
}

extern "C" void PendSV_Handler( void )
{
}

// NOLINTEND
