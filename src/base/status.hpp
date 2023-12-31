#pragma once

namespace servio::base
{

// TODO: analyze status system whenever it is finished and ready to use
enum class [[nodiscard]] status
{
        NOMINAL,
        DEGRADED,
        INOPERABLE,
};

}  // namespace servio::base
