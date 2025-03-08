#include <emlabcpp/view.h>

#pragma once

namespace servio::base
{

template < typename T >
using opt = std::optional< T >;

namespace em = emlabcpp;

struct jval_ser
{
        jval_ser( em::view< std::byte* > buff ) noexcept
          : jval_ser( em::view{ (char*) buff.begin(), (char*) buff.end() } )
        {
        }

        jval_ser( em::view< char* > buff ) noexcept
          : b( buff.begin() )
          , e( buff.end() )
        {
        }

        jval_ser& operator()( std::string_view sv ) & noexcept
        {
                return ( *this )( sv.data() );
        }

        jval_ser& operator()( char const* st ) & noexcept
        {
                add( '"' );
                cpy( st );
                add( '"' );
                return *this;
        }

        jval_ser& operator()( unsigned int x ) & noexcept
        {
                utos( p, e, x );
                return *this;
        }

        jval_ser& operator()( long unsigned int x ) & noexcept
        {
                utos( p, e, x );
                return *this;
        }

        jval_ser& operator()( bool x ) & noexcept
        {
                cpy( x ? "true" : "false" );
                return *this;
        }

        jval_ser& operator()( float x ) & noexcept
        {
                ftos( p, e, x );
                return *this;
        }

        operator em::view< std::byte* >() && noexcept
        {
                return { (std::byte*) b, (std::byte*) p };
        }

        void add( char c ) noexcept
        {
                if ( p != e )
                        *p++ = c;
        }

private:
        // XXX: this should NOT be here
        static void utos( char*& p, char* e, std::unsigned_integral auto x ) noexcept
        {
                auto add = [&]( char c ) {
                        if ( p != e )
                                *p++ = c;
                };

                char* pp = p;
                do {
                        add( '0' + ( x % 10 ) );
                        x /= 10;
                } while ( x != 0 );
                for ( char* q = p - 1; q > pp; --q, ++pp )
                        std::swap( *q, *pp );
        }

        static void ftos( char*& p, char* e, float x ) noexcept
        {
                auto add = [&]( char c ) {
                        if ( p != e )
                                *p++ = c;
                };
                if ( x < 0.0F ) {
                        add( '-' );
                        x *= -1.0F;
                }
                uint32_t y = x;
                x -= y;
                utos( p, e, y );
                add( '.' );
                static constexpr std::size_t dec_places = 6;
                for ( std::size_t i = 0; i < dec_places; ++i ) {
                        x *= 10.0F;
                        uint32_t z = x;
                        x -= z;
                        add( '0' + z );
                        if ( x == 0.00F )
                                break;
                }
        }

        void cpy( char const* st ) noexcept
        {
                while ( *st != '\0' )
                        add( *st++ );
        }

        char* b;
        char* p = b;
        char* e;
};

struct object_ser;

struct array_ser
{
        array_ser( jval_ser& s ) noexcept
          : ser( s )
        {
                ser.add( '[' );
        }

        array_ser( array_ser const& ) = delete;

        ~array_ser() noexcept
        {
                ser.add( ']' );
        }

        array_ser& operator()( auto&& v ) noexcept
        {
                delim();
                ser( v );
                return *this;
        }

        operator object_ser() noexcept;

        jval_ser& ser;

private:
        void delim() noexcept
        {
                if ( d != '\0' )
                        ser.add( d );
                d = ',';
        }

        char d = '\0';
};

struct object_ser
{
        object_ser( jval_ser& s ) noexcept
          : ser( s )
        {
                ser.add( '{' );
        }

        object_ser( object_ser const& ) = delete;

        ~object_ser() noexcept
        {
                ser.add( '}' );
        }

        object_ser& operator()( auto& k, auto& v ) noexcept
        {
                if ( delim != '\0' )
                        ser.add( delim );
                ser( k );
                ser.add( ':' );
                ser( v );
                delim = ',';
                return *this;
        }

        jval_ser& ser;

private:
        char delim = '\0';
};

inline array_ser::operator object_ser() noexcept
{
        delim();
        return { ser };
}

}  // namespace servio::base
