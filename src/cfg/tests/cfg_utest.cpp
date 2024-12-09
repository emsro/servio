#include "../../iface/def.h"
#include "../default.hpp"
#include "../key.hpp"
#include "../map.hpp"

#include <gtest/gtest.h>
#include <magic_enum/magic_enum.hpp>
#include <type_traits>
#include <typeindex>
#include <variant>

namespace servio::tests
{

template < typename T >
std::set< uint32_t > get_enum_ids()
{
        auto const           entries = magic_enum::enum_entries< T >();
        std::set< uint32_t > ids;
        for ( auto [val, name] : entries ) {
                std::ignore = name;
                ids.insert( static_cast< uint32_t >( val ) );
        }
        return ids;
}

template < typename T >
std::set< uint32_t > get_iface_ids()
{
        constexpr auto& ids = iface::field_traits< T >::ids;
        return { std::begin( ids ), std::end( ids ) };
}

TEST( CFG, cfg_proto_id_match )
{
        auto cfg_ids   = get_enum_ids< cfg::key >();
        auto iface_ids = get_iface_ids< iface::cfg >();

        EXPECT_EQ( cfg_ids, iface_ids );
}

TEST( CFG, encoder_match )
{
        auto encoder_ids = get_enum_ids< cfg::encoder_mode >();
        auto iface_ids   = get_iface_ids< iface::encoder_mode >();

        EXPECT_EQ( encoder_ids, iface_ids );
}

TEST( CFG, cfg_types )
{
        // Check that cfg::value_variant exactly represents all types in cfg::map

        cfg::map const cmap = cfg::get_default_config();

        std::set< std::type_index > used_types;

        em::protocol::for_each_register( cmap, [&]< cfg::key, typename T >( T const& ) {
                static_assert(
                    em::alternative_of< std::decay_t< T >, cfg::value_variant >,
                    "failed to find value in variant" );

                used_types.emplace( typeid( T ) );
        } );

        EXPECT_EQ( used_types.size(), std::variant_size_v< cfg::value_variant > )
            << "Not all types from variant are used";
}

}  // namespace servio::tests
