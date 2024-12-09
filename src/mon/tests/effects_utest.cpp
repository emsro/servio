#include "../effects.hpp"

#include <emlabcpp/algorithm.h>
#include <gtest/gtest.h>

namespace em = emlabcpp;

namespace servio::mon
{

TEST( mon, blinker )
{
        constexpr std::size_t  iters  = 4;
        constexpr microseconds test_t = blinker::cycle * iters;

        blinker      b;
        microseconds t;

        for ( t = 0_ms; t < test_t; t += 1_ms )
                EXPECT_FALSE( b.update( t ) );

        for ( std::size_t i = 0; i < blinker::n; i++ ) {
                b = blinker{};
                b.state.set( i );
                microseconds counter = 0_ms;
                for ( t = 0_ms; t < test_t; t += 1_ms ) {
                        if ( b.update( t ) ) {
                                counter += 1_ms;

                                EXPECT_GE( t, blinker::step * i * 2 ) << i;
                        }
                }
                EXPECT_EQ( counter, blinker::step );
        }

        for ( std::size_t i = 0; i < blinker::n; i++ ) {
                b = blinker{};
                for ( std::size_t j = 0; j <= i; j++ )
                        b.state.set( j );
                microseconds counter = 0_ms;
                for ( t = 0_ms; t < test_t; t += 1_ms )
                        if ( b.update( t ) )
                                counter += 1_ms;
                EXPECT_EQ( counter, blinker::step * ( i + 1 ) );
        }
}

TEST( mon, pulser )
{
        auto   pi = std::numbers::pi_v< float >;
        pulser p;

        for ( float i = 0.F; i < 10.F; i += 0.01F ) {
                p.val  = i;
                auto v = static_cast< uint8_t >(
                    em::map_range( std::sin( i - pi ), -1.F, 1.F, 0.F, 255.F ) );
                uint8_t v2             = p.update();
                uint8_t tolerable_diff = 10;
                EXPECT_TRUE( std::abs( v - v2 ) <= tolerable_diff )
                    << i << " - " << static_cast< int >( v ) << " - " << static_cast< int >( v2 );
        }
}

}  // namespace servio::mon
