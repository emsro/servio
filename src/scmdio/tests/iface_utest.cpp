
#include "../../drv/mock.hpp"
#include "../serial.hpp"
#include "./port_mock.hpp"

#include <gtest/gtest.h>

namespace servio::scmdio
{

using namespace avakar::literals;

template < typename T, typename F >
boost::asio::awaitable< void > tuple_coawait( T&& t, F&& f )
{
        co_await std::apply(
            [&]( auto&... its ) -> boost::asio::awaitable< void > {
                    ( co_await f( its ), ... );
            },
            t );
}

TEST( iface, base )
{
        boost::asio::io_context ctx;

        drv::mock::pwm_mot mot;
        drv::mock::pos     gp;
        drv::mock::curr    gc;
        drv::mock::vcc     gv;
        drv::mock::temp    tm;
        drv::mock::stor    sd;
        core::core         cor{ 0_ms, gv, tm, ctl::config{ .position_limits = { -2.0F, 2.0F } } };
        cfg::map           m{};
        port_mock          pm( port_mock::attrs{
                     .motor    = mot,
                     .pos_drv  = gp,
                     .curr_drv = gc,
                     .vcc_drv  = gv,
                     .temp_drv = tm,
                     .cor      = cor,
                     .cfg_map  = m,
                     .stor_drv = sd,
        } );

        bool finished = false;
        auto run_f    = [&]() -> boost::asio::awaitable< void > {
                // XXX: maybe move to vari?
                auto tpl = iface::field_tuple_t< iface::cfg >{};

                for ( auto k : iface::cfg_key::iota() )
                        co_await get_config_field( pm, k );

                co_await tuple_coawait(
                    tpl, [&]< typename F >( F& ) -> boost::asio::awaitable< void > {
                            if constexpr ( F::key == iface::cfg_key{ "encoder_mode"_a } ) {
                                    typename F::kv_type kv{ .value = "quad"_a };
                                    co_await set_config_field( pm, kv );
                            } else if constexpr ( F::key == iface::cfg_key{ "model"_a } ) {
                                    typename F::kv_type kv{ .value = "wololo" };
                                    co_await set_config_field( pm, kv );
                            } else {
                                    typename F::kv_type kv{ .value = {} };
                                    co_await set_config_field( pm, kv );
                            }
                    } );

                auto cfg_vec = co_await get_full_config( pm );
                EXPECT_EQ( cfg_vec.size(), iface::cfg_key::iota().size() );

                for ( auto k : iface::prop_key::iota() )
                        co_await get_property( pm, k );

                co_await get_property_mode( pm );
                co_await get_property_current( pm );
                co_await get_property_position( pm );
                co_await get_property_velocity( pm );

                co_await set_mode_disengaged( pm );
                co_await set_mode_power( pm, 0.0F );
                co_await set_mode_position( pm, 0.0F );
                co_await set_mode_velocity( pm, 0.0F );
                co_await set_mode_current( pm, 0.0F );

                finished = true;
        };

        co_spawn( ctx, run_f(), handle_eptr );

        ctx.run();

        EXPECT_TRUE( finished );
}

}  // namespace servio::scmdio
