#include "../../sim/motor.hpp"

#include <gtest/gtest.h>

namespace servio::sim::tests
{

TEST( sim_motor, friction_is_symmetric )
{
        for ( float const v : { 0.01F, 0.05F, 0.5F, 2.F } )
                EXPECT_FLOAT_EQ( simple_motor::resistance( -v ), -simple_motor::resistance( v ) )
                    << "v: " << v;
}

}  // namespace servio::sim::tests
