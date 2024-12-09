#include "../def.h"

#include <gtest/gtest.h>

namespace servio::iface
{

using namespace std::string_view_literals;

template < auto A >
auto kv( auto x )
{
        return kval< A, decltype( x ) >{ .value = std::move( x ) };
}

template < auto A >
auto kv()
{
        return kval< A, void >{};
}

TEST( iface, parse )
{
        auto test_f = [&]( std::string_view inpt, vari::vval< stmt > exp ) {
                auto res = parse( inpt );
                EXPECT_EQ( res, exp )
                    << "inpt: " << inpt << "\n idx: " << res.index() << " vs " << exp.index();
        };

        test_f( "mod disengaged", mode_stmt{ kv< "disengaged"_a >() } );
        test_f( "mod power 42", mode_stmt{ kv< "power"_a >( 42.0F ) } );
        test_f( "mod current -1", mode_stmt{ kv< "current"_a >( -1.0F ) } );
        test_f( "mod velocity 0.0", mode_stmt{ kv< "velocity"_a >( 0.0F ) } );
        test_f( "mod position 0", mode_stmt{ kv< "position"_a >( 0.0F ) } );
        test_f( "mod position 3.14", mode_stmt{ kv< "position"_a >( 3.14F ) } );

        test_f( "prp mode", prop_stmt{ "mode"_a } );
        test_f( "prp current", prop_stmt{ "current"_a } );
        test_f( "prp vcc", prop_stmt{ "vcc"_a } );
        test_f( "prp temp", prop_stmt{ "temp"_a } );
        test_f( "prp position", prop_stmt{ "position"_a } );
        test_f( "prp velocity", prop_stmt{ "velocity"_a } );
}

}  // namespace servio::iface
