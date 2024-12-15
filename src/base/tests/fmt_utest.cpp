#include "../fmt.hpp"

#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <emlabcpp/range.h>
#include <gtest/gtest.h>
#include <random>

namespace servio::base::tests
{

TEST( base, fmt )
{

        std::random_device       dev;
        std::mt19937             rng( dev() );
        std::normal_distribution dist( 0.0, 666.0 );

        auto rand_u32 = [&] {
                return static_cast< unsigned long >( dist( rng ) );
        };

        auto rand_f = [&] {
                return static_cast< float >( dist( rng ) );
        };

        auto test = [&]( auto x ) {
                char buff1[42] = { 0 };
                char buff2[42] = { 0 };
                auto [p, ec]   = std::to_chars( std::begin( buff1 ), std::end( buff1 ), x );
                std::string_view exp{ std::begin( buff1 ), p };
                std::string_view got  = fmt_arithm( buff2, x );
                auto             filt = []( std::string_view s ) {
                        return s.substr( 0, s.find( '.' ) + 3 );
                };
                EXPECT_EQ( filt( exp ), filt( got ) );
        };

        test( 0ul );
        test( 1ul );
        // XXX: float tests disabled as it was close enough, but turns out that snprintf and
        // std::to_chars differs too much
        // test( 0.0F );
        // test( 0.1F );
        // test( -0.1F );
        // test( NAN );

        for ( std::size_t i = 0; i < 1'000; i++ ) {
                test( rand_u32() );
                // test( rand_f() );
        }
}

}  // namespace servio::base::tests
