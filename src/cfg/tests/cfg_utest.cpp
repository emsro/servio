#include "cfg/default.hpp"
#include "cfg/key.hpp"
#include "cfg/map.hpp"
#include "io.pb.h"

#include <gtest/gtest.h>
#include <magic_enum.hpp>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <variant>

namespace servio::tests
{

TEST( CFG, cfg_proto_id_match )
{
        const auto cfg_key_entries = magic_enum::enum_entries< cfg::key >();
        const auto proto_entries   = magic_enum::enum_entries< servio::Config::PldCase >();

        std::set< uint32_t > cfg_ids;
        for ( auto [val, name] : cfg_key_entries ) {
                std::ignore = name;
                cfg_ids.insert( val );
        }

        std::set< uint32_t > proto_ids;
        for ( auto [val, name] : proto_entries ) {
                std::ignore = name;
                if ( val == servio::Config::PLD_NOT_SET )
                        continue;
                proto_ids.insert( val );
        }

        EXPECT_EQ( cfg_ids, proto_ids );
}

TEST( CFG, cfg_types )
{
        // Check that cfg::value_variant exactly represents all types in cfg::map

        const cfg::map cmap = cfg::get_default_config();

        std::set< std::type_index > used_types;

        em::protocol::for_each_register( cmap, [&]< cfg::key, typename T >( const T& ) {
                static_assert(
                    em::alternative_of< std::decay_t< T >, cfg::value_variant >,
                    "failed to find value in variant" );

                used_types.emplace( typeid( T ) );
        } );

        EXPECT_EQ( used_types.size(), std::variant_size_v< cfg::value_variant > )
            << "Not all types from variant are used";
}

}  // namespace servio::tests
