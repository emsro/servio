#include "./pid_autotune_cmd.hpp"

#include "./pid_autotune.hpp"
#include "./serial.hpp"

namespace servio::scmdio
{
boost::asio::awaitable< void > pid_autotune_current( cobs_port& port, float pwr )
{
        std::vector< double > data = co_await pid_collect(
            pwr,
            [&port]( float v ) {
                    return set_mode_power( port, v );
            },
            [&port] {
                    return get_property_current( port );
            } );

        co_await set_mode_power( port, 0.0F );
}

}  // namespace servio::scmdio
