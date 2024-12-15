#include "../converter.hpp"
#include "../linear_converter.hpp"

#include <cstdint>
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

TEST( cnv, converter )
{
        std::default_random_engine             e( 0 );
        std::binomial_distribution< uint32_t > bd( ATTEMPT_N, 0.5F );
        std::normal_distribution< float >      nd( 0.F, 1.F );

        converter cnv;

        for ( auto i : em::range( ATTEMPT_N ) ) {
                std::ignore = i;
                uint32_t lv = bd( e );
                float    la = nd( e );
                uint32_t hv = bd( e );
                float    ha = nd( e );
                cnv.set_position_cfg( lv, la, hv, ha );

                uint32_t v = bd( e );

                if ( lv == hv )
                        continue;

                float expected = em::map_range< float, float >(
                    static_cast< float >( v ),
                    static_cast< float >( lv ),
                    static_cast< float >( hv ),
                    la,
                    ha );
                EXPECT_NEAR( cnv.position.convert( v ), expected, 0.0001F )
                    << v << " (" << lv << "," << hv << ") (" << la << "," << ha << ")";
        }
}

}  // namespace servio::cnv::tests
