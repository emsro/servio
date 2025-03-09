#pragma once

#include <bit>
#include <concepts>

namespace servio::str_lib
{

using num  = int32_t;
using real = float;

namespace bits
{
constexpr bool is_letter( char c ) noexcept
{
        return ( 'a' <= c && c <= 'z' ) || ( 'A' <= c && c <= 'Z' );
}

constexpr bool is_dec( char c ) noexcept
{
        return ( '0' <= c && c <= '9' );
}

constexpr bool is_hex( char c ) noexcept
{
        return is_dec( c ) || ( 'a' <= c && c <= 'f' ) || ( 'A' <= c && c <= 'F' );
}

constexpr bool is_ws( char c ) noexcept
{
        return c == ' ' || c == '\t';
}

constexpr bool is_id_letter( char c ) noexcept
{
        return is_letter( c ) || ( c == '_' ) || is_dec( c );
}

constexpr bool hex_prefix( char const* p, char const* e )
{
        if ( p != e && *p == '0' && p++ != e && *p == 'x' )
                return true;
        return false;
}

constexpr bool has_exp_suffix( char const* p, char const* e )
{
        if ( p != e && ( *p == 'e' || *p == 'E' ) )
                return true;
        return false;
}

constexpr bool try_sign( char const*& p, char const* e, int& sign )
{
        if ( p == e )
                return false;
        if ( *p == '-' ) {
                p++;
                sign = -1;
        } else if ( *p == '+' ) {
                p++;
                sign = 1;
        } else
                sign = 1;
        return true;
}

constexpr float exp_pow( int p )
{
        if ( p >= 0 ) {
                float res = 1.0F;
                for ( int i = 0; i < p; i++ )
                        res *= 10.0F;
                return res;
        } else {
                float res = 0.1F;
                for ( int i = -1; i > p; i-- )
                        res *= 0.1F;
                return res;
        }
}

struct adder
{
        char*& p;
        char*  e;

        constexpr void operator()( char c ) noexcept
        {
                if ( p != e )
                        *p++ = c;
        }

        constexpr void ch( std::integral auto x ) noexcept
        {
                ( *this )( (char) ( '0' + x ) );
        }
};
}  // namespace bits

constexpr void u_to_s( char*& p, char* e, std::unsigned_integral auto x ) noexcept
{
        bits::adder add{ p, e };

        char* pp = p;
        do {
                add.ch( x % 10 );
                x /= 10;
        } while ( x != 0 );
        for ( char* q = p - 1; q > pp; --q, ++pp )
                std::swap( *q, *pp );
}

constexpr void f_to_s( char*& p, char* e, float x ) noexcept
{
        bits::adder add{ p, e };
        if ( x < 0.0F ) {
                add( '-' );
                x *= -1.0F;
        }
        auto y = (uint32_t) x;
        x -= (float) y;
        u_to_s( p, e, y );
        add( '.' );
        for ( std::size_t i = 0; i < 6; ++i ) {
                x *= 10.0F;
                auto z = (uint32_t) x;
                x -= (float) z;
                add.ch( z );
                if ( x == 0.00F )
                        break;
        }
}

constexpr bool hex_to_n( char const*& p, char const* e, num& x ) noexcept
{
        if ( p == e || !bits::is_hex( *p ) )
                return false;
        auto f = [&]( char c ) {
                x *= 16;
                if ( c > 'a' )
                        x += 10 + ( c - 'a' );
                else if ( c > 'A' )
                        x += 10 + ( c - 'A' );
                else
                        x += c - '0';
        };
        for ( std::size_t i = 0; i < sizeof( x ) * 2 - 1; i++ ) {
                f( *p++ );
                if ( p == e || !bits::is_hex( *p ) )
                        return true;
        }
        return false;
}

constexpr bool dec_to_n( char const*& p, char const* e, num& x ) noexcept
{
        if ( p == e || !bits::is_dec( *p ) )
                return false;
        for ( std::size_t i = 0; i < sizeof( x ) * 2 - 1; i++ ) {
                x = ( x * 10 ) + ( *p++ - '0' );
                if ( p == e || !bits::is_dec( *p ) )
                        return true;
        }
        return false;
}

constexpr bool apply_exp( char const*& p, char const* e, float& x ) noexcept
{
        int sign;
        if ( !bits::try_sign( p, e, sign ) )
                return false;
        if ( p == e )
                return false;

        num y = 0.0F;
        if ( !dec_to_n( p, e, y ) )
                return false;
        x *= bits::exp_pow( sign * y );
        return true;
}

struct s_to_nr_res
{
        bool is_num = false;

        union
        {
                num  n;
                real r;
        };
};

constexpr bool s_to_nr( char const*& p, char const* e, s_to_nr_res& x ) noexcept
{
        x.n      = 0;
        x.is_num = true;
        if ( bits::hex_prefix( p, e ) ) {
                p += 2;
                if ( !hex_to_n( p, e, x.n ) )
                        return false;
                return true;
        }

        int sign;
        if ( !bits::try_sign( p, e, sign ) )
                return false;

        if ( p == e )
                return false;

        if ( !dec_to_n( p, e, x.n ) )
                return false;

        if ( *p == '.' ) {
                float y   = 0.0F;
                float exp = 1.0F;
                while ( ++p != e && bits::is_dec( *p ) ) {
                        exp *= 10.0F;
                        y = ( y * 10 ) + ( *p - '0' );
                }
                x.r      = ( (float) sign * (float) x.n ) + ( y / exp );
                x.is_num = false;
                if ( bits::has_exp_suffix( p, e ) )
                        return apply_exp( ++p, e, x.r );
        } else if ( bits::has_exp_suffix( p, e ) ) {
                x.r      = (float) sign * (float) x.n;
                x.is_num = false;
                return apply_exp( ++p, e, x.r );
        } else {
                x.n = sign * x.n;
        }
        return true;
}

constexpr bool lex_letters( char const*& p, char const* e ) noexcept
{
        if ( p == e || !bits::is_letter( *p ) )
                return false;
        while ( p != e && bits::is_id_letter( *++p ) )
                continue;
        return true;
}

}  // namespace servio::str_lib
