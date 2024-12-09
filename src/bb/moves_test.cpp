#include "../base/base.hpp"
#include "../scmdio/cli.hpp"
#include "../scmdio/serial.hpp"
#include "bb_test_case.hpp"

#include <gtest/gtest.h>

namespace servio::bb
{

using namespace std::chrono_literals;

boost::asio::awaitable< void > test_current( boost::asio::io_context& io, scmdio::cobs_port& port )
{

        co_await scmdio::set_mode_position( port, 0.2F );

        boost::asio::steady_timer t( io, 100ms );
        co_await t.async_wait( boost::asio::use_awaitable );

        co_await scmdio::set_mode_current( port, 0.F );

        for ( float curr : { 0.1F, 0.0F, 0.15F } ) {
                co_await scmdio::set_mode_current( port, curr );

                boost::asio::steady_timer t( io, 200ms );
                co_await t.async_wait( boost::asio::use_awaitable );

                float current = co_await scmdio::get_property_current( port );

                EXPECT_NEAR( curr, current, 0.2F );
        }

        co_await scmdio::set_mode_disengaged( port );
}

boost::asio::awaitable< void > test_position( boost::asio::io_context& io, scmdio::cobs_port& port )
{

        for ( float pos : { pi / 4, pi * 3 / 2, pi } ) {

                co_await scmdio::set_mode_position( port, pos );

                boost::asio::steady_timer t( io, 2s );
                co_await t.async_wait( boost::asio::use_awaitable );

                float position = co_await scmdio::get_property_position( port );

                EXPECT_NEAR( pos, position, 0.3F );
        }

        co_await scmdio::set_mode_disengaged( port );
}

}  // namespace servio::bb

int main( int argc, char** argv )
{
        using namespace servio;
        using namespace std::chrono_literals;

        ::testing::InitGoogleTest( &argc, argv );
        bool powerless = false;

        CLI::App app{ "black box tests with basic moves" };
        scmdio::powerless_flag( app, powerless );
        scmdio::common_cli cli;
        cli.setup( app );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        if ( !powerless ) {
                bb::register_test( "moves", "current", cli, bb::test_current, 1s );
                bb::register_test( "moves", "position", cli, bb::test_position, 10s );
        }

        return RUN_ALL_TESTS();
}
