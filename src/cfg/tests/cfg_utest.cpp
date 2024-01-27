#include "cfg/key.hpp"
#include "io.pb.h"

#include <gtest/gtest.h>
#include <magic_enum.hpp>

namespace servio::tests
{

TEST( CFG, ids )
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

}  // namespace servio::tests
