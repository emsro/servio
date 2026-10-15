#include "../../base.hpp"
#include "../current/current.hpp"
#include "../position/position.hpp"

#include <cstddef>
#include <gtest/gtest.h>

namespace servio::gov::tests
{
namespace
{

template < typename G >
void expect_no_push_inside_range( G& gov )
{
        gov.get_cfg()->apply_all();
        for ( std::size_t i = 0; i <= 100; i++ ) {
                float const pos = 2 * pi * ( static_cast< float >( i ) / 100.F );
                gov.metrics_irq( 1_ms * i, pos, 0.F, false );
                EXPECT_LE( gov.derived_curr_lims.min(), 0.F ) << "pos: " << pos;
                EXPECT_GE( gov.derived_curr_lims.max(), 0.F ) << "pos: " << pos;
        }
}

}  // namespace

TEST( soft_limits, default_config_pushes_nowhere_inside_the_range )
{
        curr::_current_gov cg;
        expect_no_push_inside_range( cg );
        pos::_position_gov pg;
        expect_no_push_inside_range( pg );
}

}  // namespace servio::gov::tests
