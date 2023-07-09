#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace host
{

boost::asio::awaitable< void > query_cmd( boost::asio::serial_port& port, bool json );

boost::asio::awaitable< void > commit_cmd( boost::asio::serial_port& port );

boost::asio::awaitable< void > clear_cmd( boost::asio::serial_port& port );

boost::asio::awaitable< void >
get_cmd( boost::asio::serial_port& port, const std::string& name, bool json );

boost::asio::awaitable< void >
set_cmd( boost::asio::serial_port& port, const std::string& name, std::string value );

}  // namespace host
