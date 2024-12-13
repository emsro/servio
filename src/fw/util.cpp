#include "util.hpp"

#include "../core/globals.hpp"

namespace servio::fw
{

void stop_exec()
{
        if ( core::STOP_CALLBACK )
                core::STOP_CALLBACK();
        __asm__( "BKPT" );
        while ( true )
                asm( "nop" );
}

}  // namespace servio::fw
