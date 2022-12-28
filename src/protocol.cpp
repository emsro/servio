#include "protocol.hpp"

template class em::protocol::endpoint< master_to_servo_packet, servo_to_master_packet >;
template class em::protocol::endpoint< servo_to_master_packet, master_to_servo_packet >;
