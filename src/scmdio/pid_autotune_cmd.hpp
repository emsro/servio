#pragma once

#include "port.hpp"

namespace servio::scmdio
{

awaitable< void > pid_autotune_current( port_iface& port, float pwr );

}  // namespace servio::scmdio
