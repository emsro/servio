#include "cfg/default.hpp"
#include "cfg/storage.hpp"

#include <gtest/gtest.h>

using page = std::array< std::byte, 2048 >;

namespace std
{
std::ostream& operator<<( std::ostream& os, const std::byte& b )
{
        return os << int( b );
}
}  // namespace std

TEST( CFG, storage )
{
        EMLABCPP_INFO_LOG( "wololo" );
        std::array< page, 2 > buffer;

        std::array pages = em::map_f_to_a( buffer, [&]( page& p ) {
                return em::view{ p };
        } );

        std::optional< cfg::page > tmp;
        tmp = cfg::find_unused_page( pages );
        EXPECT_EQ( tmp.value(), pages[0] );

        tmp = cfg::find_oldest_page( pages );
        EXPECT_FALSE( tmp.has_value() );

        tmp = cfg::find_next_page( pages );
        EXPECT_EQ( tmp.value(), pages[0] );

        cfg::payload pl;
        cfg_map      cm = cfg::get_default_config();

        auto p      = tmp.value();
        auto writer = [&]( std::size_t addr, uint64_t val ) -> bool {
                std::memcpy( &p[addr], &val, sizeof( uint64_t ) );
                return true;
        };
        bool success = cfg::store( pl, cm, writer );
        EXPECT_TRUE( success );

        tmp = cfg::find_unused_page( pages );
        EXPECT_NE( tmp.value(), pages[0] );

        tmp = cfg::find_oldest_page( pages );
        EXPECT_EQ( tmp.value(), pages[0] );

        tmp = cfg::find_next_page( pages );
        EXPECT_EQ( tmp.value(), pages[1] );

        auto pl_check = [&]( const cfg::payload& pll ) -> bool {
                return pl == pll;
        };
        success = cfg::load( p, pl_check, cm );

        EXPECT_EQ( cfg::get_default_config(), cm );
}
