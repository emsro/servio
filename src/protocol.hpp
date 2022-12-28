#include "base.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/either.h>
#include <emlabcpp/protocol.h>
#include <emlabcpp/protocol/endpoint.h>
#include <emlabcpp/protocol/packet.h>

#pragma once

namespace em = emlabcpp;

enum messages_id : uint8_t
{
    SWITCH_TO_POWER_CONTROL    = 2,
    SWITCH_TO_CURRENT_CONTROL  = 3,
    SWITCH_TO_VELOCITY_CONTROL = 4,
    SWITCH_TO_POSITION_CONTROL = 5,
    SET_POWER                  = 6,
    GET_CURRENT                = 7,
    GET_VCC                    = 8,
    GET_TEMP                   = 9,
    GET_POSITION               = 10,
    ERROR_MSG                  = 13
};

template < messages_id ID, typename ProtocolType >
struct message_with_float
{
    static constexpr messages_id id = ID;

    float value;
};

template < messages_id ID >
struct command
{
    static constexpr messages_id id = ID;
};

template < messages_id ID, typename T >
struct getter
{
    static constexpr messages_id id = ID;

    T value;
};
using switch_to_power_command   = getter< SWITCH_TO_POWER_CONTROL, protocol_power >;
using switch_to_current_command = message_with_float< SWITCH_TO_CURRENT_CONTROL, protocol_current >;
using switch_to_velocity_command =
    message_with_float< SWITCH_TO_VELOCITY_CONTROL, protocol_velocity >;
using switch_to_position_command =
    message_with_float< SWITCH_TO_POSITION_CONTROL, protocol_position >;
using get_current_command  = command< GET_CURRENT >;
using get_vcc_command      = command< GET_VCC >;
using get_temp_command     = command< GET_TEMP >;
using get_position_command = command< GET_POSITION >;

using master_to_servo_group = em::protocol::tag_group<
    switch_to_power_command,
    switch_to_current_command,
    switch_to_velocity_command,
    switch_to_position_command,
    get_current_command,
    get_vcc_command,
    get_temp_command,
    get_position_command >;

using master_to_servo_variant =
    typename em::protocol::traits_for< master_to_servo_group >::value_type;

using get_current_response     = message_with_float< GET_CURRENT, protocol_current >;
using get_vcc_response         = message_with_float< GET_VCC, protocol_voltage >;
using get_temp_response        = message_with_float< GET_TEMP, protocol_temperature >;
using get_position_response    = message_with_float< GET_POSITION, protocol_position >;

using servo_to_master_group = em::protocol::tag_group<
    get_current_response,
    get_vcc_response,
    get_temp_response,
    get_position_response >;
using servo_to_master_variant =
    typename em::protocol::traits_for< servo_to_master_group >::value_type;

struct packet_def
{
    static constexpr std::endian              endianess = std::endian::little;
    static constexpr std::array< uint8_t, 4 > prefix    = { 0x98, 0xb7, 0xb0, 0x1f };
    using size_type                                     = uint16_t;
    using checksum_type                                 = uint8_t;

    static constexpr checksum_type get_checksum( const em::view< const uint8_t* > msg )
    {
        uint8_t init = 0x0;
        return em::accumulate( msg, init, [&]( uint8_t accum, uint8_t val ) -> uint8_t {
            return accum ^ val;
        } );
    }
};

using master_to_servo_packet  = em::protocol::packet< packet_def, master_to_servo_group >;
using master_to_servo_message = typename master_to_servo_packet::message_type;

using servo_to_master_packet  = em::protocol::packet< packet_def, servo_to_master_group >;
using servo_to_master_message = typename servo_to_master_packet::message_type;

using servo_endpoint  = em::protocol::endpoint< master_to_servo_packet, servo_to_master_packet >;
using master_endpoint = em::protocol::endpoint< servo_to_master_packet, master_to_servo_packet >;

extern template class em::protocol::endpoint< master_to_servo_packet, servo_to_master_packet >;
extern template class em::protocol::endpoint< servo_to_master_packet, master_to_servo_packet >;
