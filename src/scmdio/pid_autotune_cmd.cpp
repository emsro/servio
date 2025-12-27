#include "./pid_autotune_cmd.hpp"

#include "./pid_autotune.hpp"
#include "./serial.hpp"

namespace servio::scmdio
{
awaitable< void > pid_autotune_current( port_iface& port, float pwr )
{
        co_await govctl_activate( port, "power" );
        std::vector< double > data = co_await pid_collect(
            pwr,
            [&port]( float v ) {
                    return do_gov( port, "power", { "set" }, { { "goal", v } } );
            },
            [&port] -> awaitable< double > {
                    std::string val = co_await get_property( port, "current" );
                    co_return std::stod( val );
            } );

        co_await govctl_deactivate( port );

        throw std::runtime_error( "Not implemented" );
}

}  // namespace servio::scmdio
