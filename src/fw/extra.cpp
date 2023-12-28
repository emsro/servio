#include "util.hpp"

#include <cstdlib>
#include <exception>
#include <new>

// Default std::terminate handler, default implementation brings in demangling
// of C++ names, which is big.
void __gnu_cxx::__verbose_terminate_handler()
{
        fw::stop_exec();
}

void* operator new( std::size_t )
{
        fw::stop_exec();
        return NULL;
}

void operator delete( void* ) noexcept
{
        fw::stop_exec();
}

void operator delete( void*, std::size_t ) noexcept
{
        fw::stop_exec();
}

void* operator new( std::size_t, std::align_val_t )
{
        fw::stop_exec();
        return NULL;
}

void operator delete( void*, std::align_val_t ) noexcept
{
        fw::stop_exec();
}

extern "C" {

int _getpid( void )
{
        return 1;
}

void _kill( int )
{
        while ( true ) {
                asm( "nop" );
        }
}

void _exit( int )
{
        while ( true ) {
                asm( "nop" );
        }
}
}
