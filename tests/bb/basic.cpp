#include "host/cli.hpp"
#include "host/exceptions.hpp"
#include "host/serial.hpp"

#include <filesystem>
#include <gtest/gtest.h>

boost::asio::awaitable< void > test_properties_querying( boost::asio::serial_port& port )
{
        const google::protobuf::OneofDescriptor* desc =
            servio::Property::GetDescriptor()->oneof_decl( 0 );

        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );
                servio::Property prop = co_await host::load_property( port, field );
                EXPECT_EQ( static_cast< int >( prop.pld_case() ), field->number() );
        }
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

        register_test(
            "properties_querying", device.Get(), baudrate.Get(), test_properties_querying );

        return RUN_ALL_TESTS();
}
