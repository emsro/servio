#include "../iface/def.h"
#include "./exceptions.hpp"
#include "./port.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace servio::scmdio
{

template < typename T >
using opt = std::optional< T >;

// XXX: do we really want json everywhere?

boost::asio::awaitable< void > write( port_iface& port, std::string_view payload );

boost::asio::awaitable< std::string > read( port_iface& port );

boost::asio::awaitable< nlohmann::json > exchg( port_iface& port, std::string const& msg );

boost::asio::awaitable< vari::vval< iface::cfg_vals > >
get_config_field( port_iface& port, iface::cfg_key cfg );

template < auto K, typename F = typename iface::cfg_traits::ktof< K > >
boost::asio::awaitable< typename F::value_type > get_config_field( port_iface& port )
{
        using T   = typename F::value_type;
        auto vals = co_await get_config_field( port, K );
        T*   p    = vals.visit( [&]< typename KV >( KV& kv ) -> T* {
                if ( std::same_as< KV, typename F::kv_type > )
                        return &kv.value;
                return nullptr;
        } );
        if ( !p )
                throw servio_exception{};
        co_return std::move( *p );
}

boost::asio::awaitable< void >
set_config_field( port_iface& port, vari::vref< iface::cfg_vals const > val );

// XXX: nasty type
boost::asio::awaitable< std::vector< vari::vval< iface::cfg_vals > > >
get_full_config( port_iface& port );

boost::asio::awaitable< vari::vval< iface::prop_vals > >
get_property( port_iface& port, iface::prop_key );

boost::asio::awaitable< iface::mode_key > get_property_mode( port_iface& port );
boost::asio::awaitable< float >           get_property_current( port_iface& port );
boost::asio::awaitable< float >           get_property_position( port_iface& port );
boost::asio::awaitable< float >           get_property_velocity( port_iface& port );

boost::asio::awaitable< void > set_mode( port_iface& port, vari::vref< iface::mode_vals const > v );

boost::asio::awaitable< void > set_mode_disengaged( port_iface& port );
boost::asio::awaitable< void > set_mode_power( port_iface& port, float pow );
boost::asio::awaitable< void > set_mode_position( port_iface& port, float angle );
boost::asio::awaitable< void > set_mode_velocity( port_iface& port, float vel );
boost::asio::awaitable< void > set_mode_current( port_iface& port, float curr );

}  // namespace servio::scmdio
