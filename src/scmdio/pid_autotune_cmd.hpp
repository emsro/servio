#pragma once

#include "async_cobs.hpp"

namespace servio::scmdio
{

boost::asio::awaitable< void > pid_autotune_current( cobs_port& port, float pwr );

}  // namespace servio::scmdio
