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
                servio::HostToServio hts;
                hts.mutable_set_mode()->set_current( curr );
                co_await host::exchange( port, hts );

                co_await t.async_wait( boost::asio::use_awaitable );

                hts.mutable_get_property()->set_field_id( servio::Property::kCurrent );
                servio::ServioToHost sth = co_await host::exchange( port, hts );

                EXPECT_EQ( curr, sth.get_property().current() );
        }
}

boost::asio::awaitable< void >
test_position( boost::asio::io_context& io, boost::asio::serial_port& port )
{
        boost::asio::steady_timer t( io, std::chrono::seconds( 1 ) );

        for ( float pos : { pi / 4, pi * 3 / 2, pi } ) {
                servio::HostToServio hts;
                hts.mutable_set_mode()->set_position( pos );

                co_await host::exchange( port, hts );

                co_await t.async_wait( boost::asio::use_awaitable );

                hts.mutable_get_property()->set_field_id( servio::Property::kPosition );
                servio::ServioToHost sth = co_await host::exchange( port, hts );

                EXPECT_EQ( pos, sth.get_property().position() );
        }
}

int main( int argc, char** argv )
{
        ::testing::InitGoogleTest( &argc, argv );

        args::ArgumentParser parser( "basic black box tests" );
        args::HelpFlag       help( parser, "help", "Display this help menu", { 'h', "help" } );

        args::ValueFlag< std::filesystem::path > device(
            parser, "device", "Device to use", { 'd', "device" }, "/dev/ttyUSB0" );
        args::ValueFlag< unsigned > baudrate(
            parser, "baudrate", "Baudrate for communication", { 'b', "baudrate" }, 115200 );

        int res = host::parse_cli( parser, argc, argv );
        if ( res != 0 ) {
                return res;
        }

        tests::bb::register_test( "moves", "current", device.Get(), baudrate.Get(), test_current );
        tests::bb::register_test(
            "moves", "position", device.Get(), baudrate.Get(), test_position );

        return RUN_ALL_TESTS();
}
