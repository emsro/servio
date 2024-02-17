#include "cnv/linear_converter.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/range.h>
#include <gtest/gtest.h>
#include <random>

namespace servio::cnv::tests
{

namespace em = emlabcpp;

static constexpr uint32_t ATTEMPT_N = 100;

TEST( cnv, linear )
{
        std::default_random_engine             e( 0 );
        std::binomial_distribution< uint32_t > bd( ATTEMPT_N, 0.5F );

        linear_converter lc{ .offset = 0, .scale = 0 };

        for ( float s : { 0.0F, -1.0F, 1.0F, 3.1415F } )
                for ( float o : { 0.0F, -1.0F, 1.0F, 3.1415F } ) {
                        lc.offset = o;
                        lc.scale  = s;
                        for ( auto i : em::range( ATTEMPT_N ) ) {
                                std::ignore = i;

                                uint32_t v = bd( e );
                                float    expected =
                                    em::map_range( v, 0U, ATTEMPT_N, o, o + ATTEMPT_N * s );
                                EXPECT_FLOAT_EQ( lc.convert( v ), expected );
                        }
                }
}

}  // namespace servio::cnv::tests
