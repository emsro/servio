#include "metrics.hpp"

#include <gtest/gtest.h>
#include <random>

TEST( Metrics, base )
{
        std::random_device                rd{};
        std::mt19937                      gen{ rd() };
        float                             observation_deviation = 0.005f;
        std::normal_distribution< float > od{ 0, observation_deviation };

        float                     angle    = 0.f;
        float                     velocity = 0.1f;
        std::chrono::milliseconds t        = 0ms;
        std::chrono::milliseconds tstep    = 10ms;

        metrics met{ t, angle, { 0, 2 * pi } };

        for ( std::size_t i = 0; i < 200; i++ ) {
                t += tstep;
                float step_scale = 1.f + od( gen ) / 2.f;
                angle += step_scale * velocity * static_cast< float >( tstep.count() ) / 1000.f;
                angle = std::fmod( angle, 2 * pi );

                met.position_irq( t, angle );

                if ( t < 200ms ) {  // we kinda ignore first 200ms
                        continue;
                }

                EXPECT_NEAR( met.get_position(), angle, 0.01 ) << "t: " << t.count() << ", "
                                                               << "i: " << i;
        }
}
