#include "scmdio/serial.hpp"

#include <boost/asio.hpp>
#include <emlabcpp/algorithm.h>
#include <fstream>
#include <google/protobuf/util/json_util.h>

namespace em = emlabcpp;

namespace servio::scmdio
{
namespace
{
        void print_configs( const std::vector< servio::Config >& out )
        {
                std::cout << em::joined(
                                 out,
                                 std::string{ "\n" },
                                 [&]( const servio::Config& cfg ) {
                                         return cfg.ShortDebugString();
                                 } )
                          << std::endl;
        }

        void print_configs_json( const std::vector< servio::Config >& out )
        {
                google::protobuf::util::JsonPrintOptions ops;
                ops.preserve_proto_field_names = true;
                std::cout << "["
                          << em::joined(
                                 em::view( out ),
                                 std::string{ ",\n " },
                                 [&]( const servio::Config& cfg ) {
                                         std::string tmp;
                                         std::ignore = google::protobuf::util::MessageToJsonString(
                                             cfg, &tmp, ops );
                                         return tmp;
                                 } )
                          << "]" << std::endl;
        }
}  // namespace

boost::asio::awaitable< void > cfg_query_cmd( cobs_port& port, bool json )
{
        std::vector< servio::Config > out = co_await get_full_config( port );

        if ( json )
                print_configs_json( out );
        else
                print_configs( out );
}

boost::asio::awaitable< void > cfg_commit_cmd( cobs_port& port )
{
        servio::HostToServio msg;
        msg.mutable_commit_config();

        servio::ServioToHost reply = co_await exchange( port, msg );
        std::ignore                = reply;
}

boost::asio::awaitable< void > cfg_clear_cmd( cobs_port& port )
{
        servio::HostToServio msg;
        msg.mutable_clear_config();
        servio::ServioToHost reply = co_await exchange( port, msg );
        std::ignore                = reply;
}

boost::asio::awaitable< void > cfg_get_cmd( cobs_port& port, const std::string& name, bool json )
{
        const google::protobuf::Descriptor* desc = servio::Config::GetDescriptor();

        const google::protobuf::FieldDescriptor* field = desc->FindFieldByName( name );
        if ( field == nullptr ) {
                std::cerr << "Failed to find config field " << name << std::endl;
                co_return;
        }

        servio::Config cfg = co_await get_config_field( port, field );

        if ( json )
                print_configs_json( { cfg } );
        else
                print_configs( { cfg } );
}

boost::asio::awaitable< void >
cfg_set_cmd( cobs_port& port, const std::string& name, std::string value )
{

        const google::protobuf::Descriptor* desc = servio::Config::GetDescriptor();

        using field_desc        = google::protobuf::FieldDescriptor;
        const field_desc* field = desc->FindFieldByName( name );
        if ( field == nullptr ) {
                std::cerr << "Failed to find config field " << name << std::endl;
                co_return;
        }

        if ( field->is_repeated() ) {
                std::cerr << "Config field is repeated, which is not supported" << std::endl;
                co_return;
        }

        switch ( field->cpp_type() ) {
        case field_desc::CPPTYPE_INT32:
        case field_desc::CPPTYPE_INT64:
        case field_desc::CPPTYPE_UINT32:
        case field_desc::CPPTYPE_UINT64:
        case field_desc::CPPTYPE_FLOAT:
        case field_desc::CPPTYPE_DOUBLE:
                break;
        case field_desc::CPPTYPE_BOOL:
        case field_desc::CPPTYPE_ENUM:
        case field_desc::CPPTYPE_STRING:
                value = "\"" + value + "\"";
                break;
        case field_desc::CPPTYPE_MESSAGE:
                throw std::exception{};  // TODO: improve this
        }

        std::string json = "{\"" + name + "\":" + value + "}";

        servio::Config cmsg;
        auto           status = google::protobuf::util::JsonStringToMessage( json, &cmsg );
        if ( !status.ok() )
                throw std::exception{};  // TODO: improve

        servio::HostToServio msg;
        *msg.mutable_set_config() = cmsg;

        servio::ServioToHost reply = co_await exchange( port, msg );
        std::ignore                = reply;
}

boost::asio::awaitable< void > cfg_load_cmd( cobs_port& port, const std::filesystem::path& cfg )
{
        std::ifstream fd( cfg );

        auto j = nlohmann::json::parse( fd );

        for ( auto&& [key, val] : j.items() )
                co_await cfg_set_cmd( port, key, val.dump() );
}

}  // namespace servio::scmdio
