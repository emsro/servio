#include "../default.hpp"
#include "../storage.hpp"

#include <gtest/gtest.h>

using page = std::array< std::byte, 2048 >;

namespace std
{
std::ostream& operator<<( std::ostream& os, std::byte const& b )
{
        return os << int( b );
}
}  // namespace std

namespace servio::tests
{

TEST( CFG, storage )
{
        std::array< page, 2 > buffer;

        std::array pages = em::map_f_to_a( buffer, [&]( page& p ) {
                return em::view{ p };
        } );

        cfg::page const* tmp = nullptr;

        tmp = cfg::find_unused_page( pages );
        EXPECT_EQ( *tmp, pages[0] );

        tmp = cfg::find_oldest_page( pages );
        EXPECT_EQ( tmp, nullptr );

        tmp = cfg::find_next_page( pages );
        EXPECT_EQ( *tmp, pages[0] );

        cfg::payload pl;
        cfg::map     cm = cfg::get_default_config();

        auto p                 = *tmp;
        auto [success, used_p] = cfg::store( pl, &cm, p );
        EXPECT_TRUE( success );

        tmp = cfg::find_unused_page( pages );
        EXPECT_NE( *tmp, pages[0] );

        tmp = cfg::find_oldest_page( pages );
        EXPECT_EQ( *tmp, pages[0] );

        tmp = cfg::find_next_page( pages );
        EXPECT_EQ( *tmp, pages[1] );

        auto pl_check = [&]( cfg::payload const& pll ) -> bool {
                return pl == pll;
        };
        success = cfg::load( p, pl_check, cm );

        EXPECT_EQ( cfg::get_default_config(), cm );
}

}  // namespace servio::tests
