
#include "../../drv/mock.hpp"
#include "../serial.hpp"
#include "./port_mock.hpp"

#include <gtest/gtest.h>

namespace servio::scmdio
{

using namespace avakar::literals;

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
                for ( auto k : cfg::keys )
                        co_await get_config_field( pm, to_str( k ) );

                for ( auto k : cfg::keys )
                        if ( k == cfg::key::encoder_mode )
                                co_await set_config_field( pm, to_str( k ), "quad" );
                        else if ( k == cfg::key::model )
                                co_await set_config_field( pm, to_str( k ), "wololo" );
                        else if ( k == cfg::key::id || k == cfg::key::group_id )
                                co_await set_config_field( pm, to_str( k ), 0 );
                        else if ( k == cfg::key::invert_hbridge )
                                co_await set_config_field( pm, to_str( k ), false );
                        else if ( k == cfg::key::quad_encoder_range )
                                co_await set_config_field( pm, to_str( k ), 0 );
                        else
                                co_await set_config_field( pm, to_str( k ), 0.0 );

                auto cfg_vec = co_await get_full_config( pm );
                EXPECT_EQ( cfg_vec.size(), cfg::keys.size() );

                for ( auto p : iface::property_values )
                        co_await get_property( pm, to_str( p ) );

                co_await set_mode( pm, "disengaged" );
                co_await set_mode( pm, "power", 0.0 );
                co_await set_mode( pm, "position", 0.0 );
                co_await set_mode( pm, "velocity", 0.0 );
                co_await set_mode( pm, "current", 0.0 );

                finished = true;
        };

        co_spawn( ctx, run_f(), handle_eptr );

        ctx.run();

        EXPECT_TRUE( finished );
}

}  // namespace servio::scmdio
