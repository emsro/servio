#include "bb_test_case.hpp"
#include "scmdio/cli.hpp"
#include "scmdio/exceptions.hpp"
#include "scmdio/serial.hpp"

#include <emlabcpp/experimental/logging.h>
#include <filesystem>
#include <gtest/gtest.h>

namespace servio::bb
{

boost::asio::awaitable< void > check_mode( scmdio::cobs_port& port, servio::Mode m )
{
        co_await scmdio::set_mode( port, m );

        servio::Mode repl = co_await scmdio::get_property_mode( port );
        EXPECT_EQ( repl.pld_case(), m.pld_case() );
}

boost::asio::awaitable< void > disengage( boost::asio::io_context&, scmdio::cobs_port& port )
{
        servio::Mode m;
        m.mutable_disengaged();
        co_await scmdio::set_mode( port, m );
}

boost::asio::awaitable< void > test_disengaged( boost::asio::io_context&, scmdio::cobs_port& port )
{
        servio::Mode m;
        m.mutable_disengaged();
        co_await check_mode( port, m );
}

boost::asio::awaitable< void > test_power( boost::asio::io_context& io, scmdio::cobs_port& port )
{
        servio::Mode m;
        m.set_power( 0 );
        co_await check_mode( port, m );
        co_await disengage( io, port );
}

boost::asio::awaitable< void > test_current( boost::asio::io_context& io, scmdio::cobs_port& port )
{
        servio::Mode m;
        m.set_current( 0 );
        co_await check_mode( port, m );
        co_await disengage( io, port );
}

boost::asio::awaitable< void > test_velocity( boost::asio::io_context& io, scmdio::cobs_port& port )
{
        servio::Mode m;
        m.set_velocity( 0 );
        co_await check_mode( port, m );
        co_await disengage( io, port );
}

boost::asio::awaitable< void > test_position( boost::asio::io_context& io, scmdio::cobs_port& port )
{
        servio::Mode m;
        m.set_position( 0 );
        co_await check_mode( port, m );
        co_await disengage( io, port );
}

}  // namespace servio::bb

int main( int argc, char** argv )
{
        using namespace servio;
        using namespace std::chrono_literals;

        ::testing::InitGoogleTest( &argc, argv );
        bool                  powerless = false;
        std::optional< bool > verbose   = false;

        CLI::App app{ "modes related tests" };
        scmdio::powerless_flag( app, powerless );
        scmdio::verbose_opt( app, verbose );
        scmdio::common_cli cli;
        cli.setup( app );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        bb::register_test( "basic", "disengaged", cli, bb::test_disengaged, 1s );
        bb::register_test( "basic", "power", cli, bb::test_power, 1s );
        bb::register_test( "basic", "current", cli, bb::test_current, 1s );
        bb::register_test( "basic", "velocity", cli, bb::test_velocity, 1s );
        bb::register_test( "basic", "position", cli, bb::test_position, 1s );

        return RUN_ALL_TESTS();
}
