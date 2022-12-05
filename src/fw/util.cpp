#include "util.hpp"

namespace fw
{

void stop_exec()
{
    __asm__( "BKPT" );
    while ( true ) {
        asm( "nop" );
    }
}

}  // namespace fw
