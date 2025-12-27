#include "../scmdio/cli.hpp"
#include "../scmdio/serial.hpp"
#include "bb_test_case.hpp"

#include <filesystem>
#include <gtest/gtest.h>

namespace servio::bb
{

boost::asio::awaitable< void >
check_mode( scmdio::port_iface& port, std::string_view expected, std::optional< float > val = {} )
{
        co_await scmdio::govctl_activate( port, expected );
        co_await scmdio::do_gov( port, expected, { "set" }, { { "goal", *val } } );
        auto m = co_await scmdio::govctl_active( port );
        EXPECT_EQ( m, expected );
}

boost::asio::awaitable< void > test_power( boost::asio::io_context& io, scmdio::port_iface& port )
{
        co_await check_mode( port, "power", 0.0f );
        co_await govctl_deactivate( port );
}

boost::asio::awaitable< void > test_current( boost::asio::io_context& io, scmdio::port_iface& port )
{
        co_await check_mode( port, "current", 0.0f );
        co_await govctl_deactivate( port );
}

boost::asio::awaitable< void >
test_velocity( boost::asio::io_context& io, scmdio::port_iface& port )
{
        co_await check_mode( port, "velocity", 0.0f );
        co_await govctl_deactivate( port );
}

boost::asio::awaitable< void >
test_position( boost::asio::io_context& io, scmdio::port_iface& port )
{
        co_await check_mode( port, "position", 0.0f );
        co_await govctl_deactivate( port );
}

}  // namespace servio::bb

int main( int argc, char** argv )
{
        using namespace servio;
        using namespace std::chrono_literals;

        ::testing::InitGoogleTest( &argc, argv );
        boost::asio::io_context io_ctx;

        CLI::App app{ "modes related tests" };
        scmdio::verbose_opt( app );
        scmdio::char_cli port;
        scmdio::port_opts( app, port );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        bb::register_test( "basic", "power", io_ctx, port.get( io_ctx ), bb::test_power, 1s );
        bb::register_test( "basic", "current", io_ctx, port.get( io_ctx ), bb::test_current, 1s );
        bb::register_test( "basic", "velocity", io_ctx, port.get( io_ctx ), bb::test_velocity, 1s );
        bb::register_test( "basic", "position", io_ctx, port.get( io_ctx ), bb::test_position, 1s );

        return RUN_ALL_TESTS();
}
