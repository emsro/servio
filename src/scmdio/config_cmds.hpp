#include "scmdio/async_cobs.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace scmdio
{

boost::asio::awaitable< void > cfg_query_cmd( cobs_port& port, bool json );

boost::asio::awaitable< void > cfg_commit_cmd( cobs_port& port );

boost::asio::awaitable< void > cfg_clear_cmd( cobs_port& port );

boost::asio::awaitable< void > cfg_get_cmd( cobs_port& port, const std::string& name, bool json );

boost::asio::awaitable< void >
cfg_set_cmd( cobs_port& port, const std::string& name, std::string value );

}  // namespace scmdio
