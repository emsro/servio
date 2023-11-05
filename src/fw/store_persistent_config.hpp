#include "cfg/storage.hpp"

#pragma once

namespace fw
{

bool store_persistent_config( const cfg::page& page, const cfg::payload& pld, const cfg::map* cfg );

}  // namespace fw
