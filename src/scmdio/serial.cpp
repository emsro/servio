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

awaitable< vari::vval< iface::cfg_vals > > get_config_field( port_iface& port, iface::cfg_key cfg )
{
        spdlog::debug( "querying config field: {}", cfg.to_string() );

        std::string msg = std::format( "cfg get {}", cfg.to_string() );

        nlohmann::json reply = co_await exchg( port, msg );
        if ( reply.size() != 2 )
                log_error( "Expected message with one extra field, instead got: ", reply );

        spdlog::debug( "got value for {}: {}", cfg.to_string(), reply.at( 1 ) );

        auto res = kval_ser< iface::cfg_vals >::from_json( cfg.to_string(), reply.at( 1 ) );
        assert( res );
        spdlog::debug( "parsed value" );
        co_return std::move( res ).vval();
}

awaitable< void > set_config_field( port_iface& port, vari::vref< iface::cfg_vals const > val )
{
        std::string msg;
        val.visit( [&]( auto const& val ) {
                msg = std::format( "cfg set {} {}", val.key.to_string(), val.value );
        } );

        co_await exchg( port, msg );
}

awaitable< std::vector< vari::vval< iface::cfg_vals > > > get_full_config( port_iface& port )
{
        std::vector< vari::vval< iface::cfg_vals > > res;
        for ( auto f : iface::cfg_key::iota() ) {
                auto val = co_await get_config_field( port, f );
                res.emplace_back( std::move( val ) );
        }

        co_return res;
}

awaitable< vari::vval< iface::prop_vals > > get_property( port_iface& port, iface::prop_key p )
{
        std::string msg = std::format( "prop {}", p.to_string() );

        nlohmann::json reply = co_await exchg( port, msg );

        auto res = kval_ser< iface::prop_vals >::from_json( p.to_string(), reply.at( 1 ) );
        assert( res );
        co_return std::move( res ).vval();
}

namespace
{
template < auto K, typename FS, typename KV >
auto kval_filter( vari::vref< KV > var )
{
        using RT = typename iface::field_traits< FS >::template ktof< K >::value_type;
        return var.visit( [&]< typename KK >( KK& kv ) -> RT {
                if constexpr ( K.to_string() == KK::key.to_string() )
                        return kv.value;
                else
                        log_error( "Got unexpected kind of value: ", K.to_string() );
        } );
}

template < auto K >
awaitable< typename iface::prop_traits::ktof< K >::value_type > query_prop( port_iface& port )
{
        auto val = co_await get_property( port, K );
        co_return kval_filter< K, iface::property, iface::prop_vals >( val );
}

}  // namespace

awaitable< iface::mode_key > get_property_mode( port_iface& port )
{
        return query_prop< "mode"_a >( port );
}

awaitable< float > get_property_current( port_iface& port )
{
        return query_prop< "current"_a >( port );
}

awaitable< float > get_property_position( port_iface& port )
{
        return query_prop< "position"_a >( port );
}

awaitable< float > get_property_velocity( port_iface& port )
{
        return query_prop< "velocity"_a >( port );
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

awaitable< void > set_mode( port_iface& port, vari::vref< iface::mode_vals const > v )
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

awaitable< void > set_mode_disengaged( port_iface& port )
{
        co_await set_mode_raw( port, "disengaged", "" );
}

awaitable< void > set_mode_power( port_iface& port, float pow )
{
        co_await set_mode_raw( port, "power", pow );
}

awaitable< void > set_mode_position( port_iface& port, float angle )
{
        co_await set_mode_raw( port, "position", angle );
}

awaitable< void > set_mode_velocity( port_iface& port, float vel )
{
        co_await set_mode_raw( port, "velocity", vel );
}

awaitable< void > set_mode_current( port_iface& port, float curr )
{
        co_await set_mode_raw( port, "current", curr );
}

}  // namespace servio::scmdio
