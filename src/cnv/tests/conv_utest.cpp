#include "cnv/linear_converter.hpp"

#include <emlabcpp/range.h>
#include <gtest/gtest.h>
#include <random>

namespace servio::cnv::tests
{

namespace em = emlabcpp;

static constexpr std::size_t ATTEMPT_N = 100;

TEST( cnv, linear )
{
        std::default_random_engine                e( 0 );
        std::binomial_distribution< std::size_t > bd( ATTEMPT_N, 0.5F );

        linear_converter lc{ .offset = 0, .scale = 0 };

        for ( auto i : em::range( ATTEMPT_N ) )
                EXPECT_EQ( lc.convert( bd( e ) ), 0.F );
}

}  // namespace servio::cnv::tests
