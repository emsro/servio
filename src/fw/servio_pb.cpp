#include "fw/servio_pb.hpp"

#include "emlabcpp/result.h"
#include "fw/util.hpp"

#include <pb_decode.h>
#include <pb_encode.h>

namespace servio::fw
{

ServioToHost error_msg( const char* msg )
{
        ErrorMsg err_msg;
        copy_string_to( msg, strlen( msg ), err_msg.msg );

        ServioToHost reply;
        reply.error     = err_msg;
        reply.which_pld = ServioToHost_error_tag;
        return reply;
}

bool decode( em::view< const std::byte* > data, HostToServio& msg )
{
        // TODO: stream has error string that can be reported!
        pb_istream_t stream = pb_istream_from_buffer(
            reinterpret_cast< const uint8_t* >( data.begin() ), data.size() );
        const bool status = pb_decode( &stream, HostToServio_fields, &msg );
        return status;
}

bool decode( em::view< const std::byte* > data, HostToServioPacket& msg )
{
        // TODO: stream has error string that can be reported!
        pb_istream_t stream = pb_istream_from_buffer(
            reinterpret_cast< const uint8_t* >( data.begin() ), data.size() );
        const bool status = pb_decode( &stream, HostToServioPacket_fields, &msg );
        return status;
}

std::tuple< em::result, em::view< std::byte* > >
encode( em::view< std::byte* > data, const ServioToHost& msg )
{
        pb_ostream_t stream =
            pb_ostream_from_buffer( reinterpret_cast< uint8_t* >( data.begin() ), data.size() );
        const bool res = pb_encode( &stream, ServioToHost_fields, &msg );

        return { em::result::from_bool( res ), em::view_n( data.begin(), stream.bytes_written ) };
}

std::tuple< em::result, em::view< std::byte* > >
encode( em::view< std::byte* > data, const ServioToHostPacket& msg )
{
        pb_ostream_t stream =
            pb_ostream_from_buffer( reinterpret_cast< uint8_t* >( data.begin() ), data.size() );
        const bool res = pb_encode( &stream, ServioToHostPacket_fields, &msg );

        return { em::result::from_bool( res ), em::view_n( data.begin(), stream.bytes_written ) };
}

}  // namespace servio::fw
