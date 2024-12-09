#include "./serial.hpp"

#include "./async_cobs.hpp"
#include "./exceptions.hpp"
#include "./field_util.hpp"

#include <emlabcpp/view.h>

// TODO: there might be a better way of obtaining this?
constexpr std::size_t buffer_size = 1024;

namespace servio::scmdio
{

using namespace avakar::literals;

boost::asio::awaitable< void > write( cobs_port& port, std::string_view payload )
{
        EMLABCPP_DEBUG_LOG( "Sending: ", payload );

        co_await port.async_write( payload );
}

boost::asio::awaitable< std::string > read( cobs_port& port )
{

        std::array< std::byte, buffer_size > reply_buffer;
        em::view< std::byte* >               deser_msg = co_await port.async_read( reply_buffer );

        std::string res{ reinterpret_cast< char* >( deser_msg.begin() ), deser_msg.size() };
        EMLABCPP_DEBUG_LOG( "Got: ", res );
        co_return res;
}

boost::asio::awaitable< nlohmann::json > exchg( cobs_port& port, std::string const& msg )
{
        co_await write( port, msg );

        std::string reply = co_await read( port );
        co_return nlohmann::json::parse( reply );
}

boost::asio::awaitable< vari::vval< iface::cfg_vals > >
get_config_field( cobs_port& port, iface::cfg_key cfg )
{
        std::string msg = std::format( "cfg get {}", cfg.to_string() );

        nlohmann::json reply = co_await exchg( port, msg );
        if ( !reply.contains( cfg.to_string() ) ) {
                EMLABCPP_ERROR_LOG( "Expected message with cfg field, instead got: ", reply );
                throw reply_error{ "reply does not contain cfg field as it should" };
        }

        auto res = kval_ser< iface::cfg_vals >::from_json( cfg.to_string(), reply );
        assert( res );
        co_return std::move( res ).vval();
}

boost::asio::awaitable< void >
set_config_field( cobs_port& port, vari::vref< iface::cfg_vals const > val )
{
        std::string msg;
        val.visit( [&]( auto const& val ) {
                msg = std::format( "cfg set {} {}", val.key.to_string(), val.value );
        } );

        std::string res = co_await exchg( port, msg );
        if ( res != "ok" )
                throw reply_error{ "reply does not contain set_config as it should" };
}

boost::asio::awaitable< std::vector< vari::vval< iface::cfg_vals > > >
get_full_config( cobs_port& port )
{
        std::vector< vari::vval< iface::cfg_vals > > res;
        for ( auto f : iface::cfg_key::iota() ) {
                auto val = co_await get_config_field( port, f );
                res.emplace_back( std::move( val ) );
        }

        co_return res;
}

boost::asio::awaitable< vari::vval< iface::prop_vals > >
get_property( cobs_port& port, iface::prop_key p )
{
        std::string msg = std::format( "prp {}", p.to_string() );

        nlohmann::json reply = co_await exchg( port, msg );
        if ( !reply.contains( p.to_string() ) ) {
                EMLABCPP_ERROR_LOG( "Expected message with prop field, instead got: ", reply );
                throw reply_error{ "reply does not contain prop field as it should" };
        }

        auto res = kval_ser< iface::prop_vals >::from_json( p.to_string(), reply );
        assert( res );
        co_return std::move( res ).vval();
}

/// TODO: error checking for the getters
boost::asio::awaitable< iface::mode_key > get_property_mode( cobs_port& port )
{
        auto val = co_await get_property( port, "mode"_a );
        throw "XXX";
}

boost::asio::awaitable< float > get_property_current( cobs_port& port )
{
        auto val = co_await get_property( port, "current"_a );
        throw "XXX";
}

boost::asio::awaitable< float > get_property_position( cobs_port& port )
{
        auto val = co_await get_property( port, "position"_a );
        throw "XXX";
}

boost::asio::awaitable< float > get_property_velocity( cobs_port& port )
{
        auto val = co_await get_property( port, "velocity"_a );
        throw "XXX";
}

namespace
{
boost::asio::awaitable< void > set_mode_raw( cobs_port& port, std::string_view k, float v )
{
        std::string msg = std::format( "mode {} {}", k, v );
        co_await exchg( port, msg );
        // XXX: check return value
}

}  // namespace

boost::asio::awaitable< void > set_mode( cobs_port& port, vari::vref< iface::mode_vals const > v )
{
        std::string msg;
        v.visit( [&]< typename KV >( KV const& kval ) {
                if constexpr ( KV::is_void )
                        msg = std::format( "mode {}", kval.key );
                else
                        msg = std::format( "mode {} {}", kval.key, kval.value );
        } );
        co_await exchg( port, msg );
        // XXX: check return value
}

boost::asio::awaitable< void > set_mode_disengaged( cobs_port& )
{
        // XXX: it's not really easy to create instances of the type...
        // XXX: this is weird
        throw "XXX";
        /*
        auto kv = kval_ser< iface::mode_vals >::from_val(
            "disengaged", vari::vptr< iface::mode_types >{} );
        if ( kv )
                co_await set_mode( port, kv.vref() );
        else
                throw "XXX";
        */
}

boost::asio::awaitable< void > set_mode_power( cobs_port& port, float pow )
{
        co_await set_mode_raw( port, "power", pow );
}

boost::asio::awaitable< void > set_mode_position( cobs_port& port, float angle )
{
        co_await set_mode_raw( port, "position", angle );
}

boost::asio::awaitable< void > set_mode_velocity( cobs_port& port, float vel )
{
        co_await set_mode_raw( port, "velocity", vel );
}

boost::asio::awaitable< void > set_mode_current( cobs_port& port, float curr )
{
        co_await set_mode_raw( port, "current", curr );
}

}  // namespace servio::scmdio
