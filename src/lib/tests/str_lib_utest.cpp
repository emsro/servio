

#include "../str_lib.h"

#include <gtest/gtest.h>
#include <vari/vopt.h>

namespace servio::str_lib
{

template < typename T >
using opt = std::optional< T >;

struct _no_res
{
};

constexpr _no_res no_res;

namespace
{
consteval opt< s_to_nr_res > test_f( std::string_view s )
{
        char const* p = s.data();
        char const* e = s.data() + s.size();

        s_to_nr_res nrs;
        if ( s_to_nr( p, e, nrs ) )
                return nrs;
        return {};
}

template < typename T >
void check( opt< s_to_nr_res > nrs, T n )
{
        if ( nrs ) {
                if constexpr ( std::is_same_v< T, _no_res > )
                        FAIL() << "Expected no result, got " << ( nrs->is_num ? nrs->n : nrs->r );
                else if constexpr ( std::is_same_v< T, num > )
                        EXPECT_EQ( nrs->n, n );
                else if constexpr ( std::is_same_v< T, real > )
                        EXPECT_NEAR( nrs->r, n, 1e-6 );
        } else {
                if constexpr ( !std::is_same_v< T, _no_res > )
                        FAIL() << "Expected result, got none";
        }
}
}  // namespace

TEST( iface, numreal )
{

        check( test_f( "0" ), 0 );
        check( test_f( "1" ), 1 );
        check( test_f( "42" ), 42 );
        check( test_f( "-1" ), -1 );
        check( test_f( "0xFF" ), 0xFF );
        check( test_f( "0xfF" ), 0xFF );
        check( test_f( "0xFf" ), 0xFF );
        check( test_f( "0xff" ), 0xFF );
        check( test_f( ".0" ), 0.0F );
        check( test_f( "0.0" ), 0.0F );
        check( test_f( ".1" ), 0.1F );
        check( test_f( "3.14" ), 3.14F );
        check( test_f( "3e-06" ), 3e-06F );
        check( test_f( "3e-6" ), 3e-06F );
        check( test_f( "3e06" ), 3e06F );
        check( test_f( "3e6" ), 3e06F );
        check( test_f( "0.3e-06" ), 0.3e-06F );
        check( test_f( "0.3E-6" ), 0.3e-06F );
        check( test_f( "0.3e" ), no_res );
        check( test_f( "" ), no_res );
        check( test_f( "." ), no_res );
        check( test_f( ".e" ), no_res );
        check( test_f( "3e" ), no_res );
}

}  // namespace servio::str_lib
