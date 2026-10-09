#include "../converter.hpp"
#include "../linear_converter.hpp"

#include <cstdint>
#include <emlabcpp/algorithm.hpp>
#include <emlabcpp/range.hpp>
#include <gtest/gtest.h>
#include <numbers>
#include <random>
#include <tuple>

namespace servio::cnv::tests
{

namespace em = emlabcpp;

static constexpr uint32_t ATTEMPT_N = 100;

TEST( cnv, linear )
{
        std::default_random_engine             e( 0 );
        std::binomial_distribution< uint32_t > bd( ATTEMPT_N, 0.5 );

        linear_converter lc{ .offset = 0, .scale = 0 };

        for ( float const s : { 0.0F, -1.0F, 1.0F, std::numbers::pi_v< float > } )
                for ( float const o : { 0.0F, -1.0F, 1.0F, std::numbers::pi_v< float > } ) {
                        lc.offset = o;
                        lc.scale  = s;
                        for ( auto const i : em::range( ATTEMPT_N ) ) {
                                std::ignore = i;

                                uint32_t const v = bd( e );
                                float const    expected =
                                    em::map_range( v, 0U, ATTEMPT_N, o, o + ATTEMPT_N * s );
                                EXPECT_FLOAT_EQ( lc.convert( v ), expected );
                        }
                }
}

TEST( cnv, converter )
{
        std::default_random_engine             e( 0 );
        std::binomial_distribution< uint32_t > bd( ATTEMPT_N, 0.5 );
        std::normal_distribution< float >      nd( 0.F, 1.F );

        converter cnv;

        for ( auto const i : em::range( ATTEMPT_N ) ) {
                std::ignore       = i;
                uint32_t const lv = bd( e );
                float const    la = nd( e );
                uint32_t const hv = bd( e );
                float const    ha = nd( e );
                cnv.set_position_cfg( lv, la, hv, ha );

                uint32_t const v = bd( e );

                if ( lv == hv )
                        continue;

                auto const expected = em::map_range< float, float >(
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
