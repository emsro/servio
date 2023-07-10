#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace host
{

boost::asio::awaitable< void > cfg_query_cmd( boost::asio::serial_port& port, bool json );

boost::asio::awaitable< void > cfg_commit_cmd( boost::asio::serial_port& port );

boost::asio::awaitable< void > cfg_clear_cmd( boost::asio::serial_port& port );

boost::asio::awaitable< void >
cfg_get_cmd( boost::asio::serial_port& port, const std::string& name, bool json );

boost::asio::awaitable< void >
cfg_set_cmd( boost::asio::serial_port& port, const std::string& name, std::string value );

}  // namespace host
