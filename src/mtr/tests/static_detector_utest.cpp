#include "../../base.hpp"
#include "../static_detector.hpp"

#include <cstddef>
#include <gtest/gtest.h>

namespace servio::mtr::tests
{

TEST( static_detector, slow_ramp_is_moving_throughout )
{
        static_detector    det{ 0_ms, 0.F };
        float const        velocity = 0.5F;
        microseconds const tstep    = 1_ms;
        microseconds       t        = 0_ms;
        float              pos      = 0.F;
        bool               started  = false;

        for ( std::size_t i = 0; i < 2000; i++ ) {
                t += tstep;
                pos += velocity * 0.001F;
                det.update( t, pos );
                started = started || !det.is_static;
                if ( !started )
                        continue;
                EXPECT_FALSE( det.is_static ) << "t: " << t.count();
        }
        EXPECT_TRUE( started );
}

TEST( static_detector, stopping_becomes_static_after_window )
{
        static_detector det{ 0_ms, 0.F };
        det.update( 1_ms, 0.1F );
        EXPECT_FALSE( det.is_static );
        det.update( 100_ms, 0.1F );
        EXPECT_FALSE( det.is_static );
        det.update( 201_ms, 0.1F );
        EXPECT_TRUE( det.is_static );
}

TEST( static_detector, seam_jitter_is_static )
{
        static_detector det{ 0_ms, 2 * pi - 0.001F };
        microseconds    t = 0_ms;
        for ( float const p : { 0.001F, 2 * pi - 0.002F, 0.002F, 2 * pi - 0.001F } ) {
                t += 10_ms;
                det.update( t, p );
                EXPECT_TRUE( det.is_static ) << "p: " << p;
        }
}

}  // namespace servio::mtr::tests
