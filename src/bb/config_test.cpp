#include "../scmdio/cli.hpp"
#include "../scmdio/field_util.hpp"
#include "../scmdio/serial.hpp"
#include "bb_test_case.hpp"

#include <filesystem>
#include <gtest/gtest.h>

namespace avakar
{
template < auto... Ts >
std::ostream& operator<<( std::ostream& os, atom< Ts... > const& a )
{
        return os << a.to_string();
}
}  // namespace avakar

namespace servio::bb
{

void vary_value( vari::vref< iface::cfg_types > x )
{
        return x.visit(
            [&]( float& x ) {
                    x += 1;
            },
            [&]< std::size_t N >( em::string_buffer< N >& xs ) {
                    xs[0] = 'X';
            },
            [&]( iface::ec_mode_key& k ) {
                    k = iface::ec_mode_key::from_value(
                        ( k.value() + 1 ) % k.keys.size() );  // XXX: change?
            },
            [&]( bool& x ) {
                    x = !x;
            },
            [&]( uint32_t& x ) {
                    x += 1;
            } );
}

::testing::AssertionResult cmp_cfg_state(
    opt< std::size_t >                                  tested_i,
    std::vector< vari::vval< iface::cfg_vals > > const& initial,
    std::vector< vari::vval< iface::cfg_vals > > const& new_state )
{

        for ( std::size_t i = 0; i < initial.size(); i++ ) {

                bool const are_equal = initial[i] == new_state[i];
                if ( tested_i && i == *tested_i ) {
                        if ( are_equal ) {
                                return ::testing::AssertionFailure()
                                       << "The newly set key " << *tested_i
                                       << " is same as in initial setting";
                        }
                } else if ( !are_equal ) {
                        initial[i].visit( [&]< typename KV1 >( KV1& x ) {
                                new_state[i].visit( [&]( auto& y ) {
                                        return ::testing::AssertionFailure()
                                               << "Field " << KV1::key.to_string()
                                               << " changed, initial: " << x.value
                                               << " new: " << y.value;
                                } );
                        } );
                }
        }

        return ::testing::AssertionSuccess();
}

boost::asio::awaitable< void > test_config( boost::asio::io_context&, scmdio::port_iface& port )
{
        std::vector< vari::vval< iface::cfg_vals > > const cfg_vec =
            co_await scmdio::get_full_config( port );

        for ( std::size_t i = 0; i < cfg_vec.size(); i++ ) {
                co_await cfg_vec[i].visit(
                    [&]< typename KV >( KV& val ) -> boost::asio::awaitable< void > {
                            auto cpy{ val };
                            vary_value( cpy.value );
                            spdlog::info(
                                "Setting new value {}: {}, original: {}",
                                KV::key,
                                cpy.value,
                                val.value );
                            co_await scmdio::set_config_field( port, cpy );

                            EXPECT_TRUE( cmp_cfg_state(
                                i, cfg_vec, co_await scmdio::get_full_config( port ) ) );

                            spdlog::info( "Setting original value {}: {}", KV::key, val.value );

                            co_await scmdio::set_config_field( port, val );

                            auto const new_cfg_vec = co_await scmdio::get_full_config( port );

                            EXPECT_TRUE( cmp_cfg_state( {}, cfg_vec, new_cfg_vec ) );
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
        boost::asio::io_context io_ctx;

        CLI::App app{ "config tests" };
        scmdio::powerless_flag( app, powerless );
        scmdio::verbose_opt( app );
        scmdio::char_cli port;
        scmdio::port_opts( app, port );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        bb::register_test( "basic", "config", io_ctx, port.get( io_ctx ), bb::test_config, 30s );

        return RUN_ALL_TESTS();
}
