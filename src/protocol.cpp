#include "protocol.hpp"

#include <emlabcpp/protocol/packet_handler.h>

using master_to_servo_handler = em::protocol::packet_handler< master_to_servo_packet >;
using servo_to_master_handler = em::protocol::packet_handler< servo_to_master_packet >;

servo_to_master_message servo_to_master_serialize( const servo_to_master_variant& var )
{
    return servo_to_master_handler::serialize( var );
}
em::either< servo_to_master_variant, em::protocol::error_record >
servo_to_master_deserialize( const servo_to_master_message& msg )
{
    return servo_to_master_handler::extract( msg );
}

master_to_servo_message master_to_servo_serialize( const master_to_servo_variant& var )
{
    return master_to_servo_handler::serialize( var );
}
em::either< master_to_servo_variant, em::protocol::error_record >
master_to_servo_deserialize( const master_to_servo_message& msg )
{
    return master_to_servo_handler::extract( msg );
}
