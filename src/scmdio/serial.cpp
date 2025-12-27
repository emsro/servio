#include "./serial.hpp"

#include "./field_util.hpp"
#include "./port.hpp"

#include <emlabcpp/view.h>

// TODO: there might be a better way of obtaining this?
constexpr std::size_t buffer_size = 1024;

namespace servio::scmdio
{

using namespace avakar::literals;

awaitable< void > write( port_iface& port, std::string_view payload )
{
        spdlog::debug( "sending: {}", payload );

        co_await port.write_msg(
            em::view_n( reinterpret_cast< std::byte const* >( payload.data() ), payload.size() ) );
}

awaitable< std::string > read( port_iface& port )
{

        std::array< std::byte, buffer_size > reply_buffer;
        em::view< std::byte* >               deser_msg = co_await port.read_msg( reply_buffer );

        std::string res{ reinterpret_cast< char* >( deser_msg.begin() ), deser_msg.size() };
        spdlog::debug( "got: {}", res );
        co_return res;
}

awaitable< nlohmann::json > exchg( port_iface& port, std::string const& msg )
{
        co_await write( port, msg );

        std::string reply = co_await read( port );
        auto        jr    = nlohmann::json::parse( reply );
        if ( !jr.is_array() || jr.size() < 1 )
                log_error(
                    "Expected message with at least one field in array: {} input: {}", reply, msg );
        if ( jr[0] != "OK" )
                log_error( "Expected OK message, got: {}", jr.dump() );
        spdlog::debug( "json reply: {}", jr.dump() );
        co_return jr;
}

awaitable< nlohmann::json > get_config_field( port_iface& port, std::string_view name )
{
        spdlog::debug( "querying config field: {}", name );

        std::string msg = std::format( "cfg get {}", name );

        nlohmann::json reply = co_await exchg( port, msg );
        if ( reply.size() != 2 )
                log_error( "Expected message with one extra field, instead got: ", reply );

        spdlog::debug( "got value for {}: {}", name, reply.at( 1 ) );

        co_return reply.at( 1 );
}

awaitable< void >
set_config_field( port_iface& port, std::string_view name, nlohmann::json const& value )
{
        std::string msg;
        msg = std::format( "cfg set {} {}", name, value.dump() );

        co_await exchg( port, msg );
}

awaitable< std::map< std::string, nlohmann::json > > get_full_config( port_iface& port )
{
        std::map< std::string, nlohmann::json > res;
        // XXX: use the list command instead
        for ( auto k : cfg::map::keys ) {
                auto val = co_await get_config_field( port, to_str( k ) );
                res.emplace( to_str( k ), std::move( val ) );
        }

        co_return res;
}

awaitable< void > commit_config( port_iface& port )
{
        std::string msg = "cfg commit";

        auto reply = co_await exchg( port, msg );
        // XXX: check the retcode
        std::ignore = reply;
}

awaitable< nlohmann::json > get_property( port_iface& port, std::string_view prop )
{
        std::string msg = std::format( "prop {}", prop );

        nlohmann::json reply = co_await exchg( port, msg );

        co_return reply.at( 1 );
}

namespace
{
awaitable< void > set_mode_raw( port_iface& port, std::string_view k, auto v )
{
        std::string msg = std::format( "mode {} {}", k, v );
        co_await exchg( port, msg );
        // XXX: check return value
}

}  // namespace

awaitable< nlohmann::json > do_gov(
    port_iface&           port,
    std::string_view      gov,
    nlohmann::json const& args,
    nlohmann::json const& kwargs )
{
        std::string msg = std::format( "gov {}", gov );
        for ( auto& cmd : args )
                msg += std::format( " {}", cmd );
        for ( auto& [k, v] : kwargs.items() )
                msg += std::format( " {}={}", k, v.dump() );
        auto res = co_await exchg( port, msg );
        co_return res;
}

awaitable< void > govctl_activate( port_iface& port, std::string_view governor )
{
        co_await exchg( port, std::format( "govctl activate {}", governor ) );
}

awaitable< void > govctl_deactivate( port_iface& port )
{
        co_await exchg( port, "govctl deactivate" );
}

awaitable< std::string > govctl_active( port_iface& port )
{
        auto res = co_await exchg( port, "govctl active" );
        co_return ( std::string ) res.at( 1 );
}

awaitable< opt< std::string > > govctl_list( port_iface& port, int32_t index )
{
        auto res = co_await exchg( port, std::format( "govctl list {}", index ) );
        if ( res.size() < 2 )
                co_return std::nullopt;
        co_return ( std::string ) res.at( 1 );
}

}  // namespace servio::scmdio
