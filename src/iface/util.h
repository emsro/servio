
#include <string_view>
#include <vari/vopt.h>

namespace servio::iface
{

#ifdef SERVIO_NUM_CHECK
#define SERVIO_NUM_ASSERT( x ) assert( x )
#else
#define SERVIO_NUM_ASSERT( x )
#endif

using num  = int32_t;
using real = float;

constexpr bool _is_letter( char c ) noexcept
{
        return ( 'a' <= c && c <= 'z' ) || ( 'A' <= c && c <= 'Z' );
}

constexpr bool _is_dec( char c ) noexcept
{
        return ( '0' <= c && c <= '9' );
}

constexpr bool _is_hex( char c ) noexcept
{
        return _is_dec( c ) || ( 'a' <= c && c <= 'f' ) || ( 'A' <= c && c <= 'F' );
}

constexpr bool _is_ws( char c ) noexcept
{
        return c == ' ' || c == '\t';
}

constexpr bool _is_id_letter( char c ) noexcept
{
        return _is_letter( c ) || ( c == '_' ) || _is_dec( c );
}

inline vari::vopt< num > hex_numeric( char const*& p, char const* e )
{
        SERVIO_NUM_ASSERT( p != e );
        num x = 0;
        if ( !_is_hex( *p ) )
                return {};
        auto f = [&]( char c ) {
                x *= 16;
                if ( c > 'a' )
                        x += 10 + ( c - 'a' );
                else if ( c > 'A' )
                        x += 10 + ( c - 'A' );
                else
                        x += c - '0';
        };
        do
                f( *p++ );
        while ( p != e && _is_hex( *p ) );
        return x;
}

inline vari::vopt< num > dec_digits( char const*& p, char const* e )
{
        SERVIO_NUM_ASSERT( p != e );
        num x = 0;
        do
                x = ( x * 10 ) + ( *p++ - '0' );
        while ( p != e && _is_dec( *p ) );
        return x;
}

inline vari::vopt< num > dec_numeric( char const*& p, char const* e )
{
        SERVIO_NUM_ASSERT( p != e );
        if ( !_is_dec( *p ) )
                return {};
        return dec_digits( p, e );
}

constexpr bool hex_prefix( char const* p, char const* e )
{
        SERVIO_NUM_ASSERT( p != e );
        if ( *p == '0' && p++ != e && *p == 'x' )
                return true;
        return false;
}

inline vari::vopt< num, real > numreal( char const*& p, char const* e )
{
        SERVIO_NUM_ASSERT( p != e );
        if ( hex_prefix( p, e ) ) {
                p++;
                return hex_numeric( ++p, e );
        }
        int sign = 1;
        if ( *p == '-' ) {
                p++;
                sign = -1;
        }
        if ( p == e )
                return {};
        num x = 0;
        if ( *p != '.' ) {
                if ( auto r = dec_numeric( p, e ) )
                        x = *r;
                else
                        return {};
        }
        if ( *p != '.' )
                return sign * x;
        float y   = 0.0F;
        float exp = 1.0F;
        while ( ++p != e && _is_dec( *p ) ) {
                exp *= 10.0F;
                y *= 10;
                y += *p - '0';
        }
        return (float) sign * ( (float) x ) + ( y / exp );
}

constexpr bool lex_id( char const*& p, char const* e ) noexcept
{
        if ( !_is_letter( *p ) )
                return false;
        while ( p != e && _is_id_letter( *++p ) )
                continue;
        return true;
}

inline vari::vopt< std::string_view > parse_str( char const*& p, char const* e ) noexcept
{

        char const* pp = p;
        if ( *p != '"' )
                return {};
        while ( pp != e ) {
                pp++;
                if ( _is_id_letter( *pp ) || *pp == ' ' )
                        continue;
                if ( *pp != '"' )
                        return {};
                std::string_view res{ p + 1, pp };
                p = pp;
                return res;
        }
        return {};
}

}  // namespace servio::iface
