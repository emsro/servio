#include "io.pb.h"
#include "scmdio/async_cobs.hpp"

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma once

namespace scmdio
{

/// Writes a reply to servo port
boost::asio::awaitable< void > write( cobs_port& port, const servio::HostToServio& payload );

/// Reads a reply from servo over port
boost::asio::awaitable< servio::ServioToHost > read( cobs_port& port );

/// Exchanges messages with the servo over port
boost::asio::awaitable< servio::ServioToHost >
exchange( cobs_port& port, const servio::HostToServio& msg );

/// Loads a config field specified by `field` from servo over port
boost::asio::awaitable< servio::Config >
get_config_field( cobs_port& port, const google::protobuf::FieldDescriptor* field );

/// Sets config field defined by `cfg` to serv over port
boost::asio::awaitable< void > set_config_field( cobs_port& port, const servio::Config& cfg );

/// Loads full config of servo over port
boost::asio::awaitable< std::vector< servio::Config > > get_full_config( cobs_port& port );

boost::asio::awaitable< servio::Property > get_property( cobs_port& port, uint32_t field_id );

boost::asio::awaitable< servio::Property >
get_property( cobs_port& port, servio::Property::PldCase field_id );

boost::asio::awaitable< servio::Property >
get_property( cobs_port& port, const google::protobuf::FieldDescriptor* field );

boost::asio::awaitable< servio::Mode > get_property_mode( cobs_port& port );
boost::asio::awaitable< float >        get_property_current( cobs_port& port );
boost::asio::awaitable< float >        get_property_position( cobs_port& port );
boost::asio::awaitable< float >        get_property_velocity( cobs_port& port );

boost::asio::awaitable< void > set_mode( cobs_port& port, servio::Mode mode );

boost::asio::awaitable< void > set_mode_disengaged( cobs_port& port );
boost::asio::awaitable< void > set_mode_position( cobs_port& port, float angle );
boost::asio::awaitable< void > set_mode_velocity( cobs_port& port, float vel );
boost::asio::awaitable< void > set_mode_current( cobs_port& port, float curr );

}  // namespace scmdio
