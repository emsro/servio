#include "./port.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace servio::scmdio
{

awaitable< void > cfg_query_cmd( sptr< port_iface > port, bool json );

awaitable< void > cfg_commit_cmd( sptr< port_iface > port );

awaitable< void > cfg_clear_cmd( sptr< port_iface > port );

awaitable< void > cfg_get_cmd( sptr< port_iface > port, std::string const& name, bool json );

awaitable< void >
cfg_set_cmd( sptr< port_iface > port, std::string const& name, std::string value );

awaitable< void > cfg_load_cmd( sptr< port_iface > port, std::filesystem::path const& cfg );

}  // namespace servio::scmdio
