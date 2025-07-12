#include "../cfg/def.hpp"
#include "../iface/def.hpp"
#include "./port.hpp"
#include "base.hpp"

#include <boost/asio/serial_port.hpp>

#pragma once

namespace servio::scmdio
{

template < typename T >
using opt = std::optional< T >;

// XXX: do we really want json everywhere?

awaitable< void > write( port_iface& port, std::string_view payload );

awaitable< std::string > read( port_iface& port );

awaitable< nlohmann::json > exchg( port_iface& port, std::string const& msg );

awaitable< nlohmann::json > get_config_field( port_iface& port, std::string_view name );

awaitable< void >
set_config_field( port_iface& port, std::string_view name, nlohmann::json const& value );

awaitable< std::map< std::string, nlohmann::json > > get_full_config( port_iface& port );

awaitable< nlohmann::json > get_property( port_iface& port, std::string_view prop );

awaitable< void >
set_mode( port_iface& port, std::string_view mode, nlohmann::json const& arg = nullptr );

}  // namespace servio::scmdio
