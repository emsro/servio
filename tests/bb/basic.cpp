#include "bb_test_case.hpp"
#include "host/cli.hpp"
#include "host/exceptions.hpp"
#include "host/serial.hpp"

#include <emlabcpp/experimental/logging.h>
#include <filesystem>
#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>

namespace google::protobuf
{
bool operator==( const Message& lh, const Message& rh )
{
        return util::MessageDifferencer::Equals( lh, rh );
}
}  // namespace google::protobuf

boost::asio::awaitable< void >
test_properties_querying( boost::asio::io_context&, host::cobs_port& port )
{
        const google::protobuf::OneofDescriptor* desc =
            servio::Property::GetDescriptor()->oneof_decl( 0 );

        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );
                servio::Property prop = co_await host::get_property( port, field );
                EXPECT_EQ( static_cast< int >( prop.pld_case() ), field->number() );
        }
}

boost::asio::awaitable< void > check_mode( host::cobs_port& port, servio::Mode m )
{
        co_await host::set_mode( port, m );

        servio::Mode repl = co_await host::get_property_mode( port );
        EXPECT_EQ( repl.pld_case(), m.pld_case() );
}

boost::asio::awaitable< void > test_modes( boost::asio::io_context&, host::cobs_port& port )
{
        servio::Mode m;

        m.mutable_disengaged();
        co_await check_mode( port, m );

        m.set_power( 0 );
        co_await check_mode( port, m );

        m.set_current( 0 );
        co_await check_mode( port, m );

        m.set_velocity( 0 );
        co_await check_mode( port, m );

        m.set_position( 0 );
        co_await check_mode( port, m );

        m.mutable_disengaged();
        co_await check_mode( port, m );
}

servio::Config vary_value( const google::protobuf::FieldDescriptor* field, servio::Config item )
{
        using FD                                 = google::protobuf::FieldDescriptor;
        const google::protobuf::Reflection* refl = servio::Config::GetReflection();

        if ( field->is_repeated() ) {
                throw std::exception{};  // TODO: improve this
        }

        switch ( field->cpp_type() ) {
        case FD::CPPTYPE_INT32:
                refl->SetInt32( &item, field, refl->GetInt32( item, field ) + 1 );
                break;
        case FD::CPPTYPE_INT64:
                refl->SetInt64( &item, field, refl->GetInt64( item, field ) + 1 );
                break;
        case FD::CPPTYPE_UINT32:
                refl->SetUInt32( &item, field, refl->GetUInt32( item, field ) + 1 );
                break;
        case FD::CPPTYPE_UINT64:
                refl->SetUInt64( &item, field, refl->GetUInt64( item, field ) + 1 );
                break;
        case FD::CPPTYPE_FLOAT:
                refl->SetFloat( &item, field, refl->GetFloat( item, field ) + 1.f );
                break;
        case FD::CPPTYPE_DOUBLE:
                refl->SetDouble( &item, field, refl->GetDouble( item, field ) + 1.0 );
                break;
        case FD::CPPTYPE_BOOL:
                refl->SetBool( &item, field, !refl->GetBool( item, field ) );
                break;
        case FD::CPPTYPE_STRING:
                refl->SetString( &item, field, refl->GetString( item, field ) + "a" );
                break;
        case FD::CPPTYPE_ENUM:
        case FD::CPPTYPE_MESSAGE:
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
                } else {
                        if ( !are_equal ) {
                                return ::testing::AssertionFailure()
                                       << "Field " << key
                                       << " changed while only field that should've changed is "
                                       << tested_field;
                        }
                }
        }

        return ::testing::AssertionSuccess();
}

boost::asio::awaitable< void > test_config( boost::asio::io_context&, host::cobs_port& port )
{
        const std::vector< servio::Config > cfg_vec = co_await host::get_full_config( port );
        std::map< servio::Config::PldCase, servio::Config > istate_map;
        for ( const servio::Config& cfg : cfg_vec ) {
                istate_map[cfg.pld_case()] = cfg;
        }

        const google::protobuf::OneofDescriptor* desc =
            servio::Config::GetDescriptor()->oneof_decl( 0 );
        for ( int i = 0; i < desc->field_count(); i++ ) {
                const google::protobuf::FieldDescriptor* field = desc->field( i );
                auto key = static_cast< servio::Config::PldCase >( field->number() );

                servio::Config val = vary_value( field, istate_map[key] );
                EMLABCPP_INFO_LOG( "Setting new value: ", val.ShortDebugString() );
                co_await host::set_config_field( port, val );

                EXPECT_TRUE(
                    cmp_cfg_state( istate_map, key, co_await host::get_full_config( port ) ) );

                EMLABCPP_INFO_LOG(
                    "Setting original value: ", istate_map.at( key ).ShortDebugString() );
                co_await host::set_config_field( port, istate_map.at( key ) );

                const std::vector< servio::Config > new_cfg_vec =
                    co_await host::get_full_config( port );

                EXPECT_EQ( cfg_vec, new_cfg_vec );
        }
}

int main( int argc, char** argv )
{
        ::testing::InitGoogleTest( &argc, argv );

        CLI::App         app{ "basic black box tests" };
        host::common_cli cli;
        cli.setup( app );

        try {
                app.parse( argc, argv );
        }
        catch ( const CLI::ParseError& e ) {
                return app.exit( e );
        }

        tests::bb::register_test( "basic", "properties_querying", cli, test_properties_querying );
        tests::bb::register_test( "basic", "modes", cli, test_modes );
        tests::bb::register_test( "basic", "config", cli, test_config );

        return RUN_ALL_TESTS();
}
