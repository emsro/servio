#include "base.hpp"
#include "bb_test_case.hpp"
#include "host/cli.hpp"
#include "host/serial.hpp"

#include <gtest/gtest.h>

boost::asio::awaitable< void >
test_current( boost::asio::io_context& io, boost::asio::serial_port& port )
{
        boost::asio::steady_timer t( io, std::chrono::milliseconds( 100 ) );

        servio::HostToServio hts;
        hts.mutable_set_mode()->set_position( 0.1f );
        co_await host::exchange( port, hts );

        for ( float curr : { 0.2f, 0.0f, 0.3f } ) {
                co_await host::set_mode_current( port, curr );

                co_await t.async_wait( boost::asio::use_awaitable );

                float current = co_await host::get_property_current( port );

                EXPECT_EQ( curr, current );
        }
}

boost::asio::awaitable< void >
test_position( boost::asio::io_context& io, boost::asio::serial_port& port )
{
        boost::asio::steady_timer t( io, std::chrono::seconds( 1 ) );

        for ( float pos : { pi / 4, pi * 3 / 2, pi } ) {

                co_await host::set_mode_position( port, pos );

                co_await t.async_wait( boost::asio::use_awaitable );

                float position = co_await host::get_property_position( port );

                EXPECT_EQ( pos, position );
        }
}

int main( int argc, char** argv )
{
        ::testing::InitGoogleTest( &argc, argv );

        CLI::App         app{ "black box tests with basic moves" };
        host::common_cli cli;
        cli.setup( app );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        tests::bb::register_test( "moves", "current", cli, test_current );
        tests::bb::register_test( "moves", "position", cli, test_position );

        return RUN_ALL_TESTS();
}