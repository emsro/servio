#include "io.pb.h"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace host
{

/// Writes a reply to servo port
boost::asio::awaitable< void >
write( boost::asio::serial_port& port, const servio::HostToServio& msg );

/// Reads a reply from servo over port
boost::asio::awaitable< servio::ServioToHost > read( boost::asio::serial_port& port );

/// Exchanges messages with the servo over port
boost::asio::awaitable< servio::ServioToHost >
exchange( boost::asio::serial_port& port, const servio::HostToServio& msg );

/// Loads a config field specified by `field` from servo over port
boost::asio::awaitable< servio::Config >
load_config_field( boost::asio::serial_port& port, const google::protobuf::FieldDescriptor* field );

/// Sets config field defined by `cfg` to serv over port
boost::asio::awaitable< void >
set_config_field( boost::asio::serial_port& port, const servio::Config& cfg );

/// Loads full config of servo over port
boost::asio::awaitable< std::vector< servio::Config > >
load_full_config( boost::asio::serial_port& port );

boost::asio::awaitable< servio::Property >
load_property( boost::asio::serial_port& port, uint32_t field_id );

boost::asio::awaitable< servio::Property >
load_property( boost::asio::serial_port& port, servio::Property::PldCase field_id );

boost::asio::awaitable< servio::Property >
load_property( boost::asio::serial_port& port, const google::protobuf::FieldDescriptor* field );

}  // namespace host
