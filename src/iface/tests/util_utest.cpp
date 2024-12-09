
#define SERVIO_NUM_CHECK

#include "../util.h"

#include <gtest/gtest.h>

namespace vari
{
template < typename... Ts >
std::ostream& operator<<( std::ostream& os, _vval< Ts... > const& rh )
{
        rh.visit( [&]( auto& item ) {
                os << item;
        } );
        return os;
}

template < typename... Ts >
std::ostream& operator<<( std::ostream& os, _vopt< Ts... > const& rh )
{
        rh.visit(
            [&]( vari::empty_t ) {
                    os << "empty";
            },
            [&]( auto& item ) {
                    os << item;
            } );
        return os;
}
}  // namespace vari

namespace servio::iface
{

TEST( iface, numreal )
{
        auto test_f = [&]( std::string_view s, vari::vopt< num, real > const& n ) {
                char const* p = s.data();
                char const* e = s.data() + s.size();

                auto res = numreal( p, e );
                EXPECT_EQ( res, n ) << "\n inpt: " << s << "\n";
        };

        test_f( "0", 0 );
        test_f( "1", 1 );
        test_f( "42", 42 );
        test_f( "-1", -1 );
        test_f( "0xFF", 0xFF );
        test_f( "0xfF", 0xFF );
        test_f( "0xFf", 0xFF );
        test_f( "0xff", 0xFF );
        test_f( ".0", 0.0F );
        test_f( ".1", 0.1F );
        test_f( "3.14", 3.14F );
}

TEST( iface, str )
{

        auto test_f = [&]( std::string_view s, std::string_view exp ) {
                char const* p = s.data();
                char const* e = s.data() + s.size();

                auto tmp = parse_str( p, e );
                EXPECT_EQ( *tmp, exp ) << "\n inpt: " << s << "\n";
        };

        test_f( "\"wololo boo\"", "wololo boo" );
}

}  // namespace servio::iface
