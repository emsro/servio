#include "io.pb.h"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace host
{

/// Writes a reply to servo port
boost::asio::awaitable< void >
write( boost::asio::serial_port& port, const servio::HostToServio& payget );

/// Reads a reply from servo over port
boost::asio::awaitable< servio::ServioToHost > read( boost::asio::serial_port& port );

/// Exchanges messages with the servo over port
boost::asio::awaitable< servio::ServioToHost >
exchange( boost::asio::serial_port& port, const servio::HostToServio& msg );

/// Loads a config field specified by `field` from servo over port
boost::asio::awaitable< servio::Config >
get_config_field( boost::asio::serial_port& port, const google::protobuf::FieldDescriptor* field );

/// Sets config field defined by `cfg` to serv over port
boost::asio::awaitable< void >
set_config_field( boost::asio::serial_port& port, const servio::Config& cfg );

/// Loads full config of servo over port
boost::asio::awaitable< std::vector< servio::Config > >
get_full_config( boost::asio::serial_port& port );

boost::asio::awaitable< servio::Property >
get_property( boost::asio::serial_port& port, uint32_t field_id );

boost::asio::awaitable< servio::Property >
get_property( boost::asio::serial_port& port, servio::Property::PldCase field_id );

boost::asio::awaitable< servio::Property >
get_property( boost::asio::serial_port& port, const google::protobuf::FieldDescriptor* field );

boost::asio::awaitable< float > get_property_current( boost::asio::serial_port& port );
boost::asio::awaitable< float > get_property_position( boost::asio::serial_port& port );
boost::asio::awaitable< float > get_property_velocity( boost::asio::serial_port& port );

boost::asio::awaitable< void > set_mode( boost::asio::serial_port& port, servio::Mode mode );

boost::asio::awaitable< void > set_mode_disengaged( boost::asio::serial_port& port );

boost::asio::awaitable< void > set_mode_position( boost::asio::serial_port& port, float angle );
boost::asio::awaitable< void > set_mode_velocity( boost::asio::serial_port& port, float vel );
boost::asio::awaitable< void > set_mode_current( boost::asio::serial_port& port, float curr );

}  // namespace host
