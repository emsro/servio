#include "fw/servio_pb.hpp"

#include <pb_decode.h>
#include <pb_encode.h>

namespace fw
{

bool decode( em::view< std::byte* > data, HostToServio& msg )
{
        pb_istream_t stream =
            pb_istream_from_buffer( reinterpret_cast< uint8_t* >( data.begin() ), data.size() );
        bool status = pb_decode( &stream, HostToServio_fields, &msg );
        return status;
}

std::tuple< bool, em::view< std::byte* > >
encode( em::view< std::byte* > data, const ServioToHost& msg )
{
        pb_ostream_t stream =
            pb_ostream_from_buffer( reinterpret_cast< uint8_t* >( data.begin() ), data.size() );
        bool res = pb_encode( &stream, ServioToHost_fields, &msg );

        return { res, em::view_n( data.begin(), stream.bytes_written ) };
}

}  // namespace fw
