

#include "../pid_autotune.hpp"

#include <gtest/gtest.h>

namespace servio::scmdio
{

TEST( autotune, lls_symm )
{

        std::vector< double > data = { 1, 2, 3 };

        line l = linear_least_squares( data );

        EXPECT_EQ( l.off, 0.0 );
        EXPECT_EQ( l.scale, 1.0 );
}

TEST( autotune, lls_off )
{

        std::vector< double > data = { 2, 3, 4 };

        line l = linear_least_squares( data );

        EXPECT_EQ( l.off, 1.0 );
        EXPECT_EQ( l.scale, 1.0 );
}

TEST( autotune, lls_linear )
{

        std::vector< double > data = { 2, 4, 6 };

        line l = linear_least_squares( data );

        EXPECT_EQ( l.off, 0.0 );
        EXPECT_EQ( l.scale, 2.0 );
}

TEST( autotune, lls_noise )
{
        std::vector< double > data = { 6, 5, 7, 10 };

        line l = linear_least_squares( data );

        EXPECT_EQ( l.off, 3.5 );
        EXPECT_EQ( l.scale, 1.4 );
}

}  // namespace servio::scmdio
