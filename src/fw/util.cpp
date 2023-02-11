#include "util.hpp"

#include "fw/globals.hpp"

namespace fw
{

void stop_exec()
{
        if ( fw::STOP_CALLBACK ) {
                fw::STOP_CALLBACK();
        }
        __asm__( "BKPT" );
        while ( true ) {
                asm( "nop" );
        }
}

}  // namespace fw
