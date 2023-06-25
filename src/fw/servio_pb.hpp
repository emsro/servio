#include <emlabcpp/view.h>
#include <io.pb.h>

#pragma once

namespace em = emlabcpp;

namespace fw
{

/// decodes a bytes from data to HostToServio message, returns true on success
bool decode( em::view< std::byte* > data, HostToServio& msg );

/// encodes a ServioToHost message to bytes, returns succes bool and subset of data that contains
/// the serialized message
std::tuple< bool, em::view< std::byte* > >
encode( em::view< std::byte* > data, const ServioToHost& msg );

}  // namespace fw
