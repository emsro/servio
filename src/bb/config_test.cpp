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

void vary_value( std::string_view k, nlohmann::json& x )
{
        if ( x.is_number() ) {
                x = x.get< float >() + 1;
                return;
        }
        if ( x.is_string() ) {
                if ( k == "encoder_mode" ) {
                        x = x.get< std::string >() == "quad" ? "analog" : "quad";
                        return;
                }
                x = "X";
                return;
        }
        if ( x.is_boolean() ) {
                x = !x.get< bool >();
                return;
        }
        throw std::runtime_error( "Unsupported JSON type" );
}

::testing::AssertionResult cmp_cfg_state(
    std::string_view                               tested_k,
    std::map< std::string, nlohmann::json > const& initial,
    std::map< std::string, nlohmann::json > const& new_state )
{

        auto bl = initial.begin();
        auto br = new_state.begin();
        for ( ; bl != initial.end() && br != new_state.end(); ++bl, ++br ) {
                if ( bl->first != br->first ) {
                        return ::testing::AssertionFailure()
                               << "Config keys do not match: initial: " << bl->first
                               << ", new: " << br->first;
                }
                bool const are_equal = bl->second == br->second;
                if ( tested_k != "" && bl->first == tested_k ) {
                        if ( are_equal ) {
                                return ::testing::AssertionFailure()
                                       << "The newly set key " << tested_k
                                       << " is same as in initial setting";
                        }
                } else if ( !are_equal ) {
                        return ::testing::AssertionFailure()
                               << "Field " << bl->first << " changed, initial: " << bl->second
                               << " new: " << br->second;
                }
        }

        if ( bl != initial.end() || br != new_state.end() ) {
                return ::testing::AssertionFailure()
                       << "Config sizes do not match: initial: " << initial.size()
                       << ", new: " << new_state.size();
        }

        return ::testing::AssertionSuccess();
}

boost::asio::awaitable< void > test_config( boost::asio::io_context&, scmdio::port_iface& port )
{
        auto const cfg_vec = co_await scmdio::get_full_config( port );

        for ( auto const& [k, v] : cfg_vec ) {
                auto cpy{ v };
                vary_value( k, cpy );
                spdlog::info( "Setting new value {}: {}, original: {}", k, cpy, v );
                co_await scmdio::set_config_field( port, k, cpy );

                EXPECT_TRUE(
                    cmp_cfg_state( k, cfg_vec, co_await scmdio::get_full_config( port ) ) );

                spdlog::info( "Setting original value {}: {}", k, v );

                co_await scmdio::set_config_field( port, k, v );

                auto const new_cfg_vec = co_await scmdio::get_full_config( port );

                EXPECT_TRUE( cmp_cfg_state( "", cfg_vec, new_cfg_vec ) );
        }
}

}  // namespace servio::bb

int main( int argc, char** argv )
{
        using namespace servio;
        using namespace std::chrono_literals;

        ::testing::InitGoogleTest( &argc, argv );
        boost::asio::io_context io_ctx;

        CLI::App app{ "config tests" };
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
