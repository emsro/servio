#include "../iface/def.h"
#include "../scmdio/cli.hpp"
#include "../scmdio/serial.hpp"
#include "bb_test_case.hpp"

#include <filesystem>
#include <gtest/gtest.h>

namespace servio::bb
{

boost::asio::awaitable< void >
test_properties_querying( boost::asio::io_context&, scmdio::port_iface& port )
{
        for ( auto k : iface::prop_key::iota() ) {
                auto vals = co_await scmdio::get_property( port, k );
                vals.visit( [&]< auto K, typename T >( iface::kval< K, T >& ) {
                        EXPECT_EQ( K.to_string(), k.to_string() );
                } );
        }
}

}  // namespace servio::bb

int main( int argc, char** argv )
{
        using namespace servio;
        using namespace std::chrono_literals;

        ::testing::InitGoogleTest( &argc, argv );
        bool                    powerless = false;
        uint32_t                verbose   = 0;
        boost::asio::io_context io_ctx;

        CLI::App app{ "properties tests" };
        scmdio::powerless_flag( app, powerless );
        scmdio::verbose_opt( app, verbose );
        scmdio::cobs_cli port;
        scmdio::port_opts( app, port );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        bb::register_test(
            "basic",
            "properties_querying",
            io_ctx,
            port.get( io_ctx ),
            bb::test_properties_querying,
            1s );

        return RUN_ALL_TESTS();
}
