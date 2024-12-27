#include "../def.h"

#include <emlabcpp/algorithm.h>
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

TEST( iface, valid_parse )
{
        auto test_f = [&]( std::string_view inpt, vari::vval< stmt > exp ) {
                auto res = parse( inpt );
                EXPECT_EQ( res, exp )
                    << "inpt: " << inpt << "\n idx: " << res.index() << " vs " << exp.index();
        };

        test_f( "mode disengaged", mode_stmt{ kv< "disengaged"_a >() } );
        test_f( "mode power 42", mode_stmt{ kv< "power"_a >( 42.0F ) } );
        test_f( "mode current -1", mode_stmt{ kv< "current"_a >( -1.0F ) } );
        test_f( "mode velocity 0.0", mode_stmt{ kv< "velocity"_a >( 0.0F ) } );
        test_f( "mode position 0", mode_stmt{ kv< "position"_a >( 0.0F ) } );
        test_f( "mode position 3.14", mode_stmt{ kv< "position"_a >( 3.14F ) } );

        test_f( "prop mode", prop_stmt{ "mode"_a } );
        test_f( "prop current", prop_stmt{ "current"_a } );
        test_f( "prop vcc", prop_stmt{ "vcc"_a } );
        test_f( "prop temp", prop_stmt{ "temp"_a } );
        test_f( "prop position", prop_stmt{ "position"_a } );
        test_f( "prop velocity", prop_stmt{ "velocity"_a } );

        // XXX: maybe move to vari?
        field_tuple_t< cfg > tpl{};
        em::for_each( tpl, [&]< typename F >( F& ) -> void {
                using KV = typename F::kv_type;
                if constexpr ( F::key == cfg_key{ "encoder_mode"_a } )
                        test_f(
                            std::format( "cfg set {} {}", F::key.to_string(), "quad" ),
                            cfg_set_stmt{ .val = KV{ .value = "quad"_a } } );
                else if constexpr ( F::key == cfg_key{ "model"_a } )
                        test_f(
                            std::format( "cfg set {} {}", F::key.to_string(), "wololo" ),
                            cfg_set_stmt{ .val = KV{ .value = "wololo" } } );
                else if constexpr ( std::same_as< typename F::value_type, bool > )
                        test_f(
                            std::format( "cfg set {} false", F::key.to_string() ),
                            cfg_set_stmt{ .val = KV{ .value = false } } );
                else
                        test_f(
                            std::format( "cfg set {} {}", F::key.to_string(), 42 ),
                            cfg_set_stmt{ .val = KV{ .value = 42 } } );
        } );

        em::for_each( tpl, [&]< typename F >( F& ) -> void {
                test_f(
                    std::format( "cfg get {}", F::key.to_string() ), cfg_get_stmt{ .k = F::key } );
        } );

        test_f( "cfg commit", cfg_commit_stmt{} );
        test_f( "    cfg      commit       ", cfg_commit_stmt{} );
        test_f( "    cfg  \t   commit       ", cfg_commit_stmt{} );
        test_f( "    cfg\tcommit       ", cfg_commit_stmt{} );
        test_f( "cfg clear", cfg_clear_stmt{} );
}

TEST( iface, invalid_parse )
{
        auto test_f = [&]( std::string_view inpt ) {
                // XXX: maybe vval could be comparable with any T?
                vari::vval< invalid_stmt > exp = invalid_stmt{};

                auto res = parse( inpt );
                EXPECT_EQ( res, exp )
                    << "inpt: " << inpt << "\n idx: " << res.index() << " vs " << exp.index();
        };

        test_f( "" );
        for ( std::size_t i = 0; i < 255; i++ )
                test_f( std::to_string( (char) i ) );
        test_f( "mode" );
        test_f( "prop" );
        test_f( "cfg" );
        test_f( "cfg " );
        test_f( "cfg s" );
        test_f( "cfg commit boo" );
}

}  // namespace servio::iface
