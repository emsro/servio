#include "./port.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace servio::scmdio
{

boost::asio::awaitable< void > cfg_query_cmd( port_iface& port, bool json );

boost::asio::awaitable< void > cfg_commit_cmd( port_iface& port );

boost::asio::awaitable< void > cfg_clear_cmd( port_iface& port );

boost::asio::awaitable< void > cfg_get_cmd( port_iface& port, std::string const& name, bool json );

boost::asio::awaitable< void >
cfg_set_cmd( port_iface& port, std::string const& name, std::string value );

boost::asio::awaitable< void > cfg_load_cmd( port_iface& port, std::filesystem::path const& cfg );

}  // namespace servio::scmdio
