#include "sntr/central_sentry.hpp"
#include "sntr/sentry.hpp"

#include <gtest/gtest.h>

namespace servio::sntr::tests
{

static_assert( !std::is_copy_constructible_v< sentry > );
static_assert( !std::is_move_constructible_v< sentry > );
static_assert( !std::is_copy_assignable_v< sentry > );
static_assert( !std::is_move_assignable_v< sentry > );

struct test_clk : base::clk_interface
{
        base::microseconds t{ 0 };

        base::microseconds get_us() override
        {
                return t;
        }
};

struct sentry_fixture : ::testing::Test
{
        test_clk                 clk;
        std::array< record, 32 > buffer_a{};
        std::array< record, 64 > buffer_b{};
        std::size_t              stop_counter = 0;
        std::function< void() >  f            = [&]() {
                stop_counter += 1;
        };
        central_sentry central{ clk, buffer_a, buffer_b, f };
};

TEST_F( sentry_fixture, init )
{
        sentry s1( nullptr, central );
        s1.set_inoperable( 0, "" );
        EXPECT_EQ( std::string_view{ buffer_a[0].src }, std::string_view{ "unknown" } );
}

TEST_F( sentry_fixture, insert_inop )
{
        const char* name = "s1";
        sentry      s1( name, central );
        clk.t = base::microseconds{ 2342 };

        EXPECT_EQ( central.is_inoperable(), false );
        EXPECT_EQ( stop_counter, 0 );

        std::size_t eid  = 7;
        const char* emsg = "saq";
        data_type   data = std::bitset< 32 >{ 0b10101 };
        s1.set_inoperable( eid, emsg, data );

        EXPECT_EQ( central.is_inoperable(), true );
        EXPECT_EQ( stop_counter, 1 );

        EXPECT_EQ( buffer_a[0].st, record_state::SET );
        EXPECT_EQ( buffer_a[0].tp, clk.t );
        EXPECT_EQ( buffer_a[0].src, name );
        EXPECT_EQ( buffer_a[0].ecodes, ecode_set{ 1u << eid } );
        EXPECT_EQ( buffer_a[0].emsg, emsg );
        EXPECT_EQ( buffer_a[0].data, data );
}

TEST_F( sentry_fixture, insert_degr )
{
        const char*        name = "s1";
        sentry             s1( name, central );
        base::microseconds t1{ 23232 };
        clk.t = t1;

        EXPECT_EQ( central.is_inoperable(), false );
        EXPECT_EQ( stop_counter, 0 );

        std::size_t eid  = 7;
        const char* emsg = "saq";
        data_type   data = std::bitset< 32 >{ 0b10101 };
        s1.set_degraded( eid, emsg, data );

        EXPECT_EQ( central.is_inoperable(), false );
        EXPECT_EQ( stop_counter, 0 );

        EXPECT_EQ( buffer_b[0].st, record_state::SET );
        EXPECT_EQ( buffer_b[0].tp, clk.t );
        EXPECT_EQ( buffer_b[0].src, name );
        EXPECT_EQ( buffer_b[0].ecodes, ecode_set{ 1u << eid } );
        EXPECT_EQ( buffer_b[0].emsg, emsg );
        EXPECT_EQ( buffer_b[0].data, data );

        clk.t = base::microseconds{ 234243242 };

        s1.unset_degraded( eid );

        EXPECT_EQ( buffer_b[0].st, record_state::SET );
        EXPECT_EQ( buffer_b[0].tp, t1 );
        EXPECT_EQ( buffer_b[0].src, name );
        EXPECT_EQ( buffer_b[0].ecodes, ecode_set{ 1u << eid } );
        EXPECT_EQ( buffer_b[0].emsg, emsg );
        EXPECT_EQ( buffer_b[0].data, data );

        EXPECT_EQ( buffer_b[1].st, record_state::SET );
        EXPECT_EQ( buffer_b[1].tp, clk.t );
        EXPECT_EQ( buffer_b[1].src, name );
        EXPECT_EQ( buffer_b[1].ecodes, ecode_set{ 0 } );
        EXPECT_NE( buffer_b[1].emsg, buffer_b[0].emsg );
        EXPECT_EQ( buffer_b[1].data, data_type{ static_cast< uint32_t >( eid ) } );
}

TEST_F( sentry_fixture, insert_wrong_code )
{
        const char* name = "s1";
        sentry      s1( name, central );
        clk.t = base::microseconds{ 23232 };

        std::size_t eid = eid_bits * 2;
        const char* msg = "msg";
        s1.set_inoperable( eid, msg );

        EXPECT_EQ( buffer_a[0].st, record_state::SET );
        EXPECT_EQ( buffer_a[0].tp, clk.t );
        EXPECT_EQ( buffer_a[0].src, name );
        EXPECT_EQ( buffer_a[0].ecodes, ecode_set{ 1u << forbidden_eid } );
        EXPECT_NE( buffer_a[0].emsg, msg );
        EXPECT_EQ( buffer_a[0].data, data_type{ static_cast< uint32_t >( eid ) } );

        clk.t = base::microseconds{ 2323223232 };
        eid   = std::min( forbidden_eid, terminated_eid );
        s1.set_degraded( eid, msg );

        EXPECT_EQ( buffer_a[1].st, record_state::SET );
        EXPECT_EQ( buffer_a[1].tp, clk.t );
        EXPECT_EQ( buffer_a[1].src, name );
        EXPECT_EQ( buffer_a[1].ecodes, ecode_set{ 1u << forbidden_eid } );
        EXPECT_NE( buffer_a[1].emsg, msg );
        EXPECT_EQ( buffer_a[1].data, data_type{ static_cast< uint32_t >( eid ) } );
}

// TODO: test ideas:
//      test circular buffer behavior for records:
//              - that we can store degraded indefinetly
//              - that inoperable won't get removed
//      multiple sentries
//      functionality to iterate sentries from central
//      central can monitor sentries for degraded
//              - multiple sentries
//              - degrade -> undegraded + combinations of sentries
//      destructor behavior of sentry
//      turn internal err message into constants and use in tests
//      enable sanitizers for tests

}  // namespace servio::sntr::tests
