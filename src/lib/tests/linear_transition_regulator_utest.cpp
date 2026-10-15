#include "../../base.hpp"
#include "../linear_transition_regulator.hpp"

#include <gtest/gtest.h>

namespace servio::tests
{

TEST( linear_transition_regulator, decay_is_a_rate_per_second )
{
        linear_transition_regulator r;
        r.set_config( 3.F, 1.F );
        r.reset( 0_ms );

        r.update( 1_s, false );
        EXPECT_NEAR( r.state, 2.F, 1e-4F );
        r.update( 2_s, false );
        EXPECT_NEAR( r.state, 3.F, 1e-4F );
        r.update( 3_s, true );
        EXPECT_NEAR( r.state, 2.F, 1e-4F );
}

}  // namespace servio::tests
