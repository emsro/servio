#include "bb_test_case.hpp"
#include "scmdio/cli.hpp"
#include "scmdio/exceptions.hpp"
#include "scmdio/serial.hpp"

#include <emlabcpp/experimental/logging.h>
#include <filesystem>
#include <gtest/gtest.h>

namespace servio::bb
{

servio::Config vary_value( const google::protobuf::FieldDescriptor* field, servio::Config item )
{
        using field_desc                         = google::protobuf::FieldDescriptor;
        const google::protobuf::Reflection* refl = servio::Config::GetReflection();

        if ( field->is_repeated() )
                throw std::exception{};  // TODO: improve this

        switch ( field->cpp_type() ) {
        case field_desc::CPPTYPE_INT32:
                refl->SetInt32( &item, field, refl->GetInt32( item, field ) + 1 );
                break;
        case field_desc::CPPTYPE_INT64:
                refl->SetInt64( &item, field, refl->GetInt64( item, field ) + 1 );
                break;
        case field_desc::CPPTYPE_UINT32:
                refl->SetUInt32( &item, field, refl->GetUInt32( item, field ) + 1 );
                break;
        case field_desc::CPPTYPE_UINT64:
                refl->SetUInt64( &item, field, refl->GetUInt64( item, field ) + 1 );
                break;
        case field_desc::CPPTYPE_FLOAT:
                refl->SetFloat( &item, field, refl->GetFloat( item, field ) + 1.F );
                break;
        case field_desc::CPPTYPE_DOUBLE:
                refl->SetDouble( &item, field, refl->GetDouble( item, field ) + 1.0 );
                break;
        case field_desc::CPPTYPE_BOOL:
                refl->SetBool( &item, field, !refl->GetBool( item, field ) );
                break;
        case field_desc::CPPTYPE_STRING:
                refl->SetString( &item, field, refl->GetString( item, field ) + "a" );
                break;
        case field_desc::CPPTYPE_ENUM:
        case field_desc::CPPTYPE_MESSAGE:
                throw std::exception{};  // TODO: improve this
        }
        return item;
}

::testing::AssertionResult cmp_cfg_state(
    const std::map< servio::Config::PldCase, servio::Config >& initial,
    servio::Config::PldCase                                    tested_field,
    const std::vector< servio::Config >&                       new_state )
{

        for ( const servio::Config& cfg : new_state ) {
                const servio::Config::PldCase key = cfg.pld_case();

                const bool are_equal = cfg == initial.at( key );
                if ( key == tested_field ) {
                        if ( are_equal ) {
                                return ::testing::AssertionFailure()
                                       << "The newly set key " << key
                                       << " is same as in initial setting";
                        }
                } else if ( !are_equal ) {
                        return ::testing::AssertionFailure()
                               << "Field " << key
                               << " changed while only field that should've changed is "
                               << tested_field;
                }
        }

        return ::testing::AssertionSuccess();
}

boost::asio::awaitable< void > test_config( boost::asio::io_context&, scmdio::cobs_port& port )
{
        const std::vector< servio::Config > cfg_vec = co_await scmdio::get_full_config( port );
        std::map< servio::Config::PldCase, servio::Config > istate_map;
        for ( const servio::Config& cfg : cfg_vec )
                istate_map[cfg.pld_case()] = cfg;

        const google::protobuf::OneofDescriptor* desc =
            servio::Config::GetDescriptor()->oneof_decl( 0 );
        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );
                auto key = static_cast< servio::Config::PldCase >( field->number() );

                servio::Config val = vary_value( field, istate_map[key] );
                EMLABCPP_INFO_LOG( "Setting new value: ", val.ShortDebugString() );
                co_await scmdio::set_config_field( port, val );

                EXPECT_TRUE(
                    cmp_cfg_state( istate_map, key, co_await scmdio::get_full_config( port ) ) );

                EMLABCPP_INFO_LOG(
                    "Setting original value: ", istate_map.at( key ).ShortDebugString() );
                co_await scmdio::set_config_field( port, istate_map.at( key ) );

                const std::vector< servio::Config > new_cfg_vec =
                    co_await scmdio::get_full_config( port );

                EXPECT_EQ( cfg_vec, new_cfg_vec );
        }
}

}  // namespace servio::bb

int main( int argc, char** argv )
{
        using namespace servio;
        using namespace std::chrono_literals;

        ::testing::InitGoogleTest( &argc, argv );
        bool                  powerless = false;
        std::optional< bool > verbose   = false;

        CLI::App app{ "config tests" };
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

        bb::register_test( "basic", "config", cli, bb::test_config, 10s );

        return RUN_ALL_TESTS();
}
