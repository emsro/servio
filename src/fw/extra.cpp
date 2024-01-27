#include "util.hpp"

#include <cstdlib>
#include <exception>
#include <new>

// Default std::terminate handler, default implementation brings in demangling
// of C++ names, which is big.
void __gnu_cxx::__verbose_terminate_handler()
{
        servio::fw::stop_exec();
}

void* operator new( std::size_t )
{
        servio::fw::stop_exec();
        return NULL;
}

void operator delete( void* ) noexcept
{
        servio::fw::stop_exec();
}

void operator delete( void*, std::size_t ) noexcept
{
        servio::fw::stop_exec();
}

void* operator new( std::size_t, std::align_val_t )
{
        servio::fw::stop_exec();
        return NULL;
}

void operator delete( void*, std::align_val_t ) noexcept
{
        servio::fw::stop_exec();
}

extern "C" {

// TODO: have to re-think this, we should not block inside a loop coz that might mean hbridge is
// still active - find a decent way to disable hbridge AND than switch to infinite loop

int _getpid( void )
{
        return 1;
}

void _kill( int )
{
        while ( true )
                asm( "nop" );
}

void _exit( int )
{
        while ( true )
                asm( "nop" );
}

int _write( int, char*, int )
{
        return -1;
}

int _read()
{
        return -1;
}

int _close()
{
        return -1;
}

int _lseek()
{
        return -1;
}
}
