#include "sntr/central_sentry.hpp"

#include <gtest/gtest.h>

namespace servio::sntr::tests
{

static_assert( !std::is_copy_constructible_v< central_sentry > );
static_assert( !std::is_move_constructible_v< central_sentry > );
static_assert( !std::is_copy_assignable_v< central_sentry > );
static_assert( !std::is_move_assignable_v< central_sentry > );

struct test_clk : drv::clk_iface
{
        base::microseconds t{ 0 };

        base::microseconds get_us() override
        {
                return t;
        }
};

struct central_sentry_fixture : ::testing::Test
{
        test_clk                 clk;
        std::array< record, 1 >  buffer_a{};
        std::array< record, 32 > buffer_b{};
        std::array< record, 64 > buffer_c{};
};

TEST( central_sentry, record )
{
        record rec;
        EXPECT_EQ( rec.st, record_state::UNSET );
        EXPECT_EQ( rec.tp, base::microseconds{ 0 } );
        EXPECT_EQ( rec.src, nullptr );
        EXPECT_EQ( rec.ecodes, ecode_set{ 0 } );
        EXPECT_EQ( rec.emsg, nullptr );
        EXPECT_EQ( rec.data, data_type{ std::monostate{} } );
}

TEST_F( central_sentry_fixture, empty_inop )
{
        std::size_t fired_count = 0;
        auto        f           = [&]() {
                fired_count += 1;
        };
        central_sentry cs{ clk, {}, buffer_a, f };

        EXPECT_EQ( fired_count, 1 );
        EXPECT_TRUE( cs.is_inoperable() );
}

TEST_F( central_sentry_fixture, empty_degr )
{
        std::size_t fired_count = 0;
        auto        f           = [&]() {
                fired_count += 1;
        };
        central_sentry cs{ clk, buffer_a, {}, f };

        EXPECT_EQ( fired_count, 1 );
        EXPECT_TRUE( cs.is_inoperable() );
}

TEST_F( central_sentry_fixture, one_deg_insert )
{
        std::size_t fired_count = 0;
        auto        f           = [&]() {
                fired_count += 1;
        };
        central_sentry cs{ clk, buffer_b, buffer_c, f };

        EXPECT_EQ( fired_count, 0 );
        EXPECT_FALSE( cs.is_inoperable() );

        const char* src    = "t1";
        ecode_set   ecodes = 0b1010;
        const char* emsg   = "esmg";
        data_type   data   = 42;
        clk.t              = base::microseconds{ 666 };

        cs.report_degraded( src, ecodes, emsg, data );

        EXPECT_EQ( fired_count, 0 );
        EXPECT_FALSE( cs.is_inoperable() );

        EXPECT_EQ( buffer_c[0].st, record_state::SET );
        EXPECT_EQ( buffer_c[0].tp, clk.t );
        EXPECT_EQ( buffer_c[0].src, src );
        EXPECT_EQ( buffer_c[0].ecodes, ecodes );
        EXPECT_EQ( buffer_c[0].emsg, emsg );
        EXPECT_EQ( buffer_c[0].data, data );

        EXPECT_EQ( buffer_c[1].st, record_state::UNSET );
}

TEST_F( central_sentry_fixture, one_inop_insert )
{
        std::size_t fired_count = 0;
        auto        f           = [&]() {
                fired_count += 1;
        };
        central_sentry cs{ clk, buffer_b, buffer_c, f };

        EXPECT_EQ( fired_count, 0 );
        EXPECT_FALSE( cs.is_inoperable() );

        const char* src    = "t1";
        ecode_set   ecodes = 0b1010;
        const char* emsg   = "esmg";
        data_type   data   = 42;
        clk.t              = base::microseconds{ 666 };

        cs.report_inoperable( src, ecodes, emsg, data );

        EXPECT_EQ( fired_count, 1 );
        EXPECT_TRUE( cs.is_inoperable() );

        EXPECT_EQ( buffer_b[0].st, record_state::SET );
        EXPECT_EQ( buffer_b[0].tp, clk.t );
        EXPECT_EQ( buffer_b[0].src, src );
        EXPECT_EQ( buffer_b[0].ecodes, ecodes );
        EXPECT_EQ( buffer_b[0].emsg, emsg );
        EXPECT_EQ( buffer_b[0].data, data );

        EXPECT_EQ( buffer_b[1].st, record_state::UNSET );

        cs.report_degraded( src, ecodes, emsg, data );
        EXPECT_TRUE( cs.is_inoperable() );
}

}  // namespace servio::sntr::tests
