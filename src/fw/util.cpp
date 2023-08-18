#include "util.hpp"

#include "globals.hpp"

namespace fw
{

void stop_exec()
{
        if ( STOP_CALLBACK ) {
                STOP_CALLBACK();
        }
        __asm__( "BKPT" );
        while ( true ) {
                asm( "nop" );
        }
}

}  // namespace fw
