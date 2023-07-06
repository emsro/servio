#include "host/cli.hpp"
#include "host/exceptions.hpp"
#include "host/serial.hpp"

#include <filesystem>
#include <gtest/gtest.h>

namespace host
{
boost::asio::awaitable< float >
load_property( boost::asio::serial_port& port, servio::Property prop )
{
        servio::HostToServio hts;
        hts.mutable_get_property()->set_property( prop );
        servio::ServioToHost sth = co_await exchange( port, hts );
        if ( !sth.has_get_property() ) {
                throw reply_error{ "reply does not contain get_property as it should" };
        }
        co_return sth.get_property().value();
}
}  // namespace host

boost::asio::awaitable< void > test_properties( boost::asio::serial_port& port )
{
        // TODO: this is kinda it, as the load_property error checking is enough
        float current = co_await host::load_property( port, servio::PROPERTY_CURRENT );
        EXPECT_NE( current, 0.f );
        float vcc = co_await host::load_property( port, servio::PROPERTY_VCC );
        EXPECT_NE( vcc, 0.f );
        float temp = co_await host::load_property( port, servio::PROPERTY_TEMP );
        EXPECT_NE( temp, 0.f );
        float position = co_await host::load_property( port, servio::PROPERTY_POSITION );
        EXPECT_NE( position, 0.f );
}

using test_signature = boost::asio::awaitable< void >( boost::asio::serial_port& );

struct bb_test_case : ::testing::Test
{
        bb_test_case(
            std::filesystem::path           device,
            unsigned                        baudrate,
            std::function< test_signature > test )
          : port( context, device )
          , test( test )
        {
                port.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );
        }

        void TestBody() final
        {
                std::exception_ptr excep_ptr;
                co_spawn( port.get_executor(), test( port ), [&]( std::exception_ptr ptr ) {
                        excep_ptr = ptr;
                } );
                context.run();

                if ( excep_ptr ) {
                        std::rethrow_exception( excep_ptr );
                }
        }

        boost::asio::io_context         context;
        boost::asio::serial_port        port;
        std::function< test_signature > test;
};

void register_test(
    const char*                     name,
    std::filesystem::path           device,
    unsigned                        baudrate,
    std::function< test_signature > test )
{
        ::testing::RegisterTest( "blackbox", name, nullptr, nullptr, __FILE__, __LINE__, [=] {
                return new bb_test_case( device, baudrate, test );
        } );
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

        register_test( "properties", device.Get(), baudrate.Get(), test_properties );

        return RUN_ALL_TESTS();
}
