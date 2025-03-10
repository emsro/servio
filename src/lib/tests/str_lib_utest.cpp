

#include "../str_lib.h"

#include <emlabcpp/algorithm.h>
#include <gtest/gtest.h>
#include <regex>
#include <source_location>
#include <vari/vopt.h>

namespace servio::str_lib
{

namespace em = emlabcpp;

template < typename T >
using opt = std::optional< T >;

struct _invalid
{
};

constexpr _invalid invalid;

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
void check(
    opt< s_to_nr_res >   nrs,
    T                    n,
    std::source_location loc = std::source_location::current() )
{
        static constexpr bool is_valid = !std::same_as< T, _invalid >;
        if ( nrs ) {
                if constexpr ( !is_valid )
                        FAIL() << "Expected no result, got " << ( nrs->is_num ? nrs->n : nrs->r )
                               << "\n loc: " << loc.file_name() << ":" << loc.line();
                else if constexpr ( std::is_same_v< T, num > )
                        EXPECT_EQ( nrs->n, n )
                            << "\n loc: " << loc.file_name() << ":" << loc.line();
                else if constexpr ( std::is_same_v< T, real > )
                        EXPECT_NEAR( nrs->r, n, 1e-6 )
                            << "\n loc: " << loc.file_name() << ":" << loc.line();
        } else {
                if constexpr ( !std::is_same_v< T, _invalid > )
                        FAIL() << "Expected result, got none"
                               << "\n loc: " << loc.file_name() << ":" << loc.line();
        }
}

#define TEST_INPT( inp, n ) check( test_f( inp ), n )

}  // namespace

TEST( lib, numreal )
{
        TEST_INPT( "-1", -1 );
        TEST_INPT( ".", invalid );
        TEST_INPT( ".EEE", invalid );
        TEST_INPT( ".m", invalid );
        TEST_INPT( ".0", invalid );
        TEST_INPT( ".1", invalid );
        TEST_INPT( ".e", invalid );
        TEST_INPT( "0", 0 );
        TEST_INPT( "0.0", 0.0F );
        TEST_INPT( "0.3E-6", 0.3e-06F );
        TEST_INPT( "0.3e", invalid );
        TEST_INPT( "0.3e-06", 0.3e-06F );
        TEST_INPT( "0xFF", 0xFF );
        TEST_INPT( "0xFf", 0xFF );
        TEST_INPT( "0xfF", 0xFF );
        TEST_INPT( "0xff", 0xFF );
        TEST_INPT( "0x", invalid );
        TEST_INPT( "1", 1 );
        TEST_INPT( "3.14", 3.14F );
        TEST_INPT( "3e", invalid );
        TEST_INPT( "3e-06", 3e-06F );
        TEST_INPT( "3e-6", 3e-06F );
        TEST_INPT( "3e06", 3e06F );
        TEST_INPT( "3e6", 3e06F );
        TEST_INPT( "42", 42 );
        TEST_INPT( "m", invalid );
        TEST_INPT( "m   42", invalid );
        TEST_INPT( "", invalid );
        TEST_INPT( "-0", 0 );
        TEST_INPT( "-0.0", -0.0F );
        TEST_INPT( "0.0000001", 0.0000001F );
        TEST_INPT( "-0.0000001", -0.0000001F );
        TEST_INPT( "1e-7", 1e-7F );
        TEST_INPT( "-1e-7", -1e-7F );
        TEST_INPT( "1E7", 1e7F );
        TEST_INPT( "-1E7", -1e7F );
        TEST_INPT( "1.0e+7", 1.0e+7F );
        TEST_INPT( "-1.0e+7", -1.0e+7F );
        TEST_INPT( "1.0e-7", 1.0e-7F );
        TEST_INPT( "-1.0e-7", -1.0e-7F );
        TEST_INPT( "1.0E+7", 1.0e+7F );
        TEST_INPT( "-1.0E+7", -1.0e+7F );
        TEST_INPT( "1.0E-7", 1.0e-7F );
        TEST_INPT( "-1.0E-7", -1.0e-7F );
        TEST_INPT( "1.0e7", 1.0e7F );
        TEST_INPT( "-1.0e7", -1.0e7F );
        TEST_INPT( "1.0E7", 1.0e7F );
        TEST_INPT( "-1.0E7", -1.0e7F );
        TEST_INPT( "1.0e+07", 1.0e+07F );
        TEST_INPT( "-1.0e+07", -1.0e+07F );
        TEST_INPT( "1.0e-07", 1.0e-07F );
        TEST_INPT( "-1.0e-07", -1.0e-07F );
        TEST_INPT( "1.0E+07", 1.0e+07F );
        TEST_INPT( "-1.0E+07", -1.0e+07F );
        TEST_INPT( "1.0E-07", 1.0e-07F );
        TEST_INPT( "-1.0E-07", -1.0e-07F );
        TEST_INPT( "0x0", 0x0 );
        TEST_INPT( "0x1", 0x1 );
        TEST_INPT( "0x11FFFFFFF", invalid );
        TEST_INPT( "0x7FFFFFFF", 0x7FFFFFFF );
        TEST_INPT( "2147483647", std::numeric_limits< int32_t >::max() );
        TEST_INPT( "2147483648", invalid );
        TEST_INPT( "-2147483647", -2147483647 );
        TEST_INPT( "-2147483648", std::numeric_limits< int32_t >::min() );
        TEST_INPT( "-2147483649", invalid );
        TEST_INPT( "0x80000000", invalid );       // Out of 32-bit integer range
        TEST_INPT( "0xFFFFFFFF", invalid );       // Out of 32-bit integer range
        TEST_INPT( "0x100000000", invalid );      // Out of 32-bit integer range
        TEST_INPT( "-10000000000000", invalid );  // Out of 32-bit integer range
        TEST_INPT( "103333330000000", invalid );  // Out of 32-bit integer range
        TEST_INPT(
            "1.7976931348623157e+308",
            std::numeric_limits< double >::infinity() );  // Out of 32-bit float range
        TEST_INPT(
            "-1.7976931348623157e+308",
            -std::numeric_limits< double >::infinity() );  // Out of 32-bit float range
        TEST_INPT( "2.2250738585072014e-308", 0.0F );      // Out of 32-bit float range
        TEST_INPT( "-2.2250738585072014e-308", -0.0F );    // Out of 32-bit float rang
}

}  // namespace servio::str_lib
