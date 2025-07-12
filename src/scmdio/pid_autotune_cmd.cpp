#include "./pid_autotune_cmd.hpp"

#include "./pid_autotune.hpp"
#include "./serial.hpp"

namespace servio::scmdio
{
awaitable< void > pid_autotune_current( port_iface& port, float pwr )
{
        std::vector< double > data = co_await pid_collect(
            pwr,
            [&port]( float v ) {
                    return set_mode( port, "power", std::to_string( v ) );
            },
            [&port] -> awaitable< double > {
                    std::string val = co_await get_property( port, "current" );
                    co_return std::stod( val );
            } );

        co_await set_mode( port, "power", "0.0" );

        throw std::runtime_error( "Not implemented" );
}

}  // namespace servio::scmdio
