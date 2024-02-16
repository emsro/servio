#include "bb_test_case.hpp"
#include "scmdio/cli.hpp"
#include "scmdio/exceptions.hpp"
#include "scmdio/serial.hpp"

#include <emlabcpp/experimental/logging.h>
#include <filesystem>
#include <gtest/gtest.h>

namespace servio::bb
{

boost::asio::awaitable< void >
test_properties_querying( boost::asio::io_context&, scmdio::cobs_port& port )
{
        const google::protobuf::OneofDescriptor* desc =
            servio::Property::GetDescriptor()->oneof_decl( 0 );

        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );
                servio::Property prop = co_await scmdio::get_property( port, field );
                EXPECT_EQ( static_cast< int >( prop.pld_case() ), field->number() );
        }
}

}  // namespace servio::bb

int main( int argc, char** argv )
{
        using namespace servio;
        using namespace std::chrono_literals;

        ::testing::InitGoogleTest( &argc, argv );
        bool powerless = false;
        bool verbose   = false;

        CLI::App app{ "properties tests" };
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

        bb::register_test( "basic", "properties_querying", cli, bb::test_properties_querying, 1s );

        return RUN_ALL_TESTS();
}
