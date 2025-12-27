
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
        core::core         cor{ 0_ms, gv, tm };
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

        gov::create_governors( cor.gov_, cor.gov_mem );

        bool finished = false;
        auto run_f    = [&]() -> boost::asio::awaitable< void > {
                for ( auto k : cfg::map::keys )
                        co_await get_config_field( pm, to_str( k ) );

                for ( auto k : cfg::map::keys )
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
                EXPECT_EQ( cfg_vec.size(), cfg::map::keys.size() );

                for ( auto p : iface::property_values )
                        co_await get_property( pm, to_str( p ) );

                co_await govctl_activate( pm, "power" );
                co_await govctl_deactivate( pm );
                co_await govctl_activate( pm, "position" );
                co_await govctl_activate( pm, "velocity" );
                co_await govctl_activate( pm, "current" );

                finished = true;
        };

        co_spawn( ctx, run_f(), handle_eptr );

        ctx.run();

        EXPECT_TRUE( finished );
}

}  // namespace servio::scmdio
