#include "../iface/def.h"
#include "./async_cobs.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace servio::scmdio
{

template < typename T >
using opt = std::optional< T >;

// XXX: do we really want json everywhere?

boost::asio::awaitable< void > write( cobs_port& port, std::string_view payload );

boost::asio::awaitable< std::string > read( cobs_port& port );

boost::asio::awaitable< nlohmann::json > exchg( cobs_port& port, std::string const& msg );

boost::asio::awaitable< vari::vval< iface::cfg_vals > >
get_config_field( cobs_port& port, iface::cfg_key cfg );

boost::asio::awaitable< void >
set_config_field( cobs_port& port, vari::vref< iface::cfg_vals const > val );

// XXX: nasty type
boost::asio::awaitable< std::vector< vari::vval< iface::cfg_vals > > >
get_full_config( cobs_port& port );

boost::asio::awaitable< vari::vval< iface::prop_vals > >
get_property( cobs_port& port, iface::prop_key );

boost::asio::awaitable< iface::mode_key > get_property_mode( cobs_port& port );
boost::asio::awaitable< float >           get_property_current( cobs_port& port );
boost::asio::awaitable< float >           get_property_position( cobs_port& port );
boost::asio::awaitable< float >           get_property_velocity( cobs_port& port );

boost::asio::awaitable< void > set_mode( cobs_port& port, vari::vref< iface::mode_vals const > v );

boost::asio::awaitable< void > set_mode_disengaged( cobs_port& port );
boost::asio::awaitable< void > set_mode_power( cobs_port& port, float pow );
boost::asio::awaitable< void > set_mode_position( cobs_port& port, float angle );
boost::asio::awaitable< void > set_mode_velocity( cobs_port& port, float vel );
boost::asio::awaitable< void > set_mode_current( cobs_port& port, float curr );

}  // namespace servio::scmdio
