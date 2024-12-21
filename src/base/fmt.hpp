#include <emlabcpp/view.h>

#pragma once

namespace servio::base
{

namespace em = emlabcpp;

struct json_ser
{
        json_ser( em::view< char* > buff, char const* prefix )
          : b( buff.begin() )
          , e( buff.end() )
        {
                ( *this )( prefix, '[' );
        }

        json_ser& operator()( std::string_view sv ) &
        {
                return ( *this )( sv.data() );
        }

        json_ser& operator()( char const* st, char c = ',' ) &
        {
                add( c );
                add( '"' );
                cpy( st );
                add( '"' );
                return *this;
        }

        json_ser& operator()( unsigned int x ) &
        {
                add( ',' );
                fmt( "%u", x );
                return *this;
        }

        json_ser& operator()( long unsigned int x ) &
        {
                add( ',' );
                fmt( "%lu", x );
                return *this;
        }

        json_ser& operator()( bool x ) &
        {
                add( ',' );
                cpy( x ? "true" : "false" );
                return *this;
        }

        json_ser& operator()( float x ) &
        {
                add( ',' );
                fmt( "%f", x );
                return *this;
        }

        template < typename T >
        json_ser&& operator()( T st ) &&
        {
                ( *this )( (T&&) st );
                return std::move( *this );
        }

        operator std::string_view() &&
        {
                add( ']' );
                return { b, p };
        }

private:
        void fmt( char const* fmt, auto& x )
        {
                std::size_t size = static_cast< std::size_t >( e - p );
                int         res  = snprintf( p, size, fmt, x );

                size = std::min( (std::size_t) std::max( res, 0 ), size );
                p += size;
        }

        void cpy( char const* st )
        {
                while ( *st != '\0' )
                        add( *st++ );
        }

        void add( char c )
        {
                if ( p != e )
                        *p++ = c;
        }

        char* b;
        char* p = b;
        char* e;
};
}  // namespace servio::base
