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

constexpr bool hex_to_n( char const*& p, char const* e, int32_t& x ) noexcept
{
        if ( p == e || !bits::is_hex( *p ) )
                return false;
        uint32_t y = 0u;
        for ( std::size_t i = 0; i < sizeof( x ) * 2; i++ ) {
                char c = *p++;
                y *= 16u;
                if ( c > 'a' )
                        y += 10u + static_cast< uint32_t >( c - 'a' );
                else if ( c > 'A' )
                        y += 10u + static_cast< uint32_t >( c - 'A' );
                else
                        y += static_cast< uint32_t >( c - '0' );
                if ( y > std::numeric_limits< num >::max() )
                        return false;
                else if ( p != e && bits::is_hex( *p ) )
                        continue;
                x = (int32_t) y;
                return true;
        }
        return false;
}

namespace bits
{
consteval uint32_t i32_lowest_as_u32()
{
        int64_t x = std::numeric_limits< int32_t >::lowest();
        return (uint32_t) -x;
}
}  // namespace bits

constexpr bool dec_to_n( char const*& p, char const* e, int32_t& x ) noexcept
{
        int sign;
        if ( !bits::try_sign( p, e, sign ) )
                return false;
        if ( p == e || !bits::is_dec( *p ) )
                return false;
        uint32_t y = 0;
        for ( ;; ) {
                uint32_t yy = y * 10;
                if ( yy < y )
                        return false;
                y = ( y * 10u ) + static_cast< uint32_t >( *p++ - '0' );
                if ( sign == 1 && y > std::numeric_limits< int32_t >::max() )
                        return false;
                else if ( sign == -1 && y > bits::i32_lowest_as_u32() )
                        return false;
                else if ( p != e && bits::is_dec( *p ) )
                        continue;
                else if ( y == bits::i32_lowest_as_u32() && sign == -1 )
                        x = std::numeric_limits< int32_t >::lowest();
                else {
                        x = (int32_t) y;
                        x *= sign;
                }
                return true;
        }
        return false;
}

constexpr bool apply_exp( char const*& p, char const* e, float& x ) noexcept
{
        if ( p == e )
                return false;

        num y = 0.0F;
        if ( !dec_to_n( p, e, y ) )
                return false;
        x *= bits::exp_pow( y );
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
                x.r      = (float) x.n + ( y / exp );
                x.is_num = false;
                if ( bits::has_exp_suffix( p, e ) )
                        return apply_exp( ++p, e, x.r );
        } else if ( bits::has_exp_suffix( p, e ) ) {
                x.r      = (float) x.n;
                x.is_num = false;
                return apply_exp( ++p, e, x.r );
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
