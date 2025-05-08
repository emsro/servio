#include "../../iface/def.hpp"
#include "../def.hpp"

#include <gtest/gtest.h>
#include <magic_enum/magic_enum.hpp>
#include <type_traits>
#include <typeindex>
#include <variant>

namespace servio::tests
{

std::set< uint32_t > get_cfg_ids()
{
        std::set< uint32_t > ids;
        for ( auto const& d : cfg::keys )
                ids.insert( static_cast< uint32_t >( d ) );
        return ids;
}

template < typename T >
std::set< uint32_t > get_iface_ids()
{
        constexpr auto& ids = iface::field_traits< T >::ids;
        return { std::begin( ids ), std::end( ids ) };
}

std::vector< uint32_t > diff( std::set< uint32_t > const& lh, std::set< uint32_t > const& rh )
{
        std::vector< uint32_t > res;
        std::set_difference(
            lh.begin(), lh.end(), rh.begin(), rh.end(), std::inserter( res, res.begin() ) );
        return res;
}

TEST( CFG, cfg_proto_id_match )
{
        auto cfg_ids   = get_cfg_ids();
        auto iface_ids = get_iface_ids< iface::cfg >();

        EXPECT_EQ( cfg_ids, iface_ids )
            << "diff: "
            << em::joined( diff( cfg_ids, iface_ids ), std::string{ "," }, [&]( uint32_t x ) {
                       return std::to_string( x );
               } );
}

}  // namespace servio::tests
