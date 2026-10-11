#include "../../cfg/base.hpp"
#include "../../cfg/handler.hpp"
#include "../../drv/mock.hpp"
#include "../../status.hpp"
#include "../current/cfg.hpp"
#include "../current/current.hpp"
#include "../governor_manager.hpp"
#include "../velocity/velocity.hpp"

#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <zll.hpp>

namespace servio::gov::tests
{

TEST( gov_cfg, apply_all_applies_the_map )
{
        curr::_current_gov g;
        g.get_cfg()->apply_all();

        EXPECT_EQ( g.pid.cfg.coefficients.p, g.cfg.curr_loop_p );
        EXPECT_EQ( g.pid.cfg.coefficients.i, g.cfg.curr_loop_i / curr::_current_gov::pid_r );
        EXPECT_EQ( g.pid.cfg.coefficients.d, g.cfg.curr_loop_d * curr::_current_gov::pid_r );
}

TEST( gov_cfg, commit_and_reload )
{
        drv::mock::stor stor;
        float const     p = 7.F;
        {
                curr::_current_gov         g;
                zll::ll_list< cfg::iface > cfgs;
                cfgs.link_back( *g.get_cfg() );
                ASSERT_FALSE( g.get_cfg()->on_cmd_set( "curr_loop_p", p ) );
                ASSERT_EQ( stor.store_cfg( cfgs ), status::success );
        }

        curr::_current_gov         g;
        zll::ll_list< cfg::iface > cfgs;
        cfgs.link_back( *g.get_cfg() );
        ASSERT_EQ( stor.load_cfg( cfgs ), status::success );
        g.get_cfg()->apply_all();

        EXPECT_EQ( g.cfg.curr_loop_p, p );
        EXPECT_EQ( g.pid.cfg.coefficients.p, p );
}

TEST( gov_cfg, sections_keep_maps_apart )
{
        drv::mock::stor stor;
        float const     curr_p = 7.F;
        float const     vel_p  = 5.F;
        {
                curr::_current_gov         cg;
                vel::_velocity_gov         vg;
                zll::ll_list< cfg::iface > cfgs;
                cfgs.link_back( *cg.get_cfg() );
                cfgs.link_back( *vg.get_cfg() );
                cg.get_cfg()->section = cg.cfg_section();
                vg.get_cfg()->section = vg.cfg_section();
                ASSERT_FALSE( cg.get_cfg()->on_cmd_set( "curr_loop_p", curr_p ) );
                ASSERT_FALSE( vg.get_cfg()->on_cmd_set( "vel_loop_p", vel_p ) );
                ASSERT_EQ( stor.store_cfg( cfgs ), status::success );
        }

        curr::_current_gov         cg;
        vel::_velocity_gov         vg;
        zll::ll_list< cfg::iface > cfgs;
        cfgs.link_back( *cg.get_cfg() );
        cfgs.link_back( *vg.get_cfg() );
        cg.get_cfg()->section = cg.cfg_section();
        vg.get_cfg()->section = vg.cfg_section();
        ASSERT_EQ( stor.load_cfg( cfgs ), status::success );

        EXPECT_EQ( cg.cfg.curr_loop_p, curr_p );
        EXPECT_EQ( vg.cfg.vel_loop_p, vel_p );
}

TEST( gov_cfg, storage_seen_tracking )
{
        curr::cfg::map                 m;
        cfg::handler< curr::cfg::map > h{ m, +[]( curr::cfg::key ) {} };
        h.storage_reset_seen();

        uint32_t const first = curr::cfg::map::ids.front();
        EXPECT_TRUE( h.storage_mark_seen( first ) );
        EXPECT_FALSE( h.storage_mark_seen( first ) );
        EXPECT_FALSE( h.storage_mark_seen( 0xFFFFU ) );

        std::size_t taken = 0;
        while ( auto const id = h.storage_take_unseen() ) {
                EXPECT_NE( *id, first );
                EXPECT_FALSE( h.storage_mark_seen( *id ) );
                taken += 1;
        }
        EXPECT_EQ( taken, curr::cfg::map::ids.size() - 1 );

        h.storage_reset_seen();
        EXPECT_TRUE( h.storage_mark_seen( first ) );
}

TEST( gov_cfg, default_sections_do_not_collide )
{
        governors_ctx const govs;
        EXPECT_TRUE( govs.valid() );
}

TEST( gov_cfg, colliding_sections_are_rejected )
{
        curr::cfg::map                 m;
        cfg::handler< curr::cfg::map > a{ m, +[]( curr::cfg::key ) {} };
        cfg::handler< curr::cfg::map > b{ m, +[]( curr::cfg::key ) {} };
        a.section = 1;
        b.section = 1;
        zll::ll_list< cfg::iface > cfgs;
        cfgs.link_back( a );
        cfgs.link_back( b );
        EXPECT_EQ( check_cfg_sections( cfgs ), status::error );
}

TEST( gov_cfg, root_section_is_rejected )
{
        curr::cfg::map                 m;
        cfg::handler< curr::cfg::map > a{ m, +[]( curr::cfg::key ) {} };
        a.section = cfg::root_cfg_section;
        zll::ll_list< cfg::iface > cfgs;
        cfgs.link_back( a );
        EXPECT_EQ( check_cfg_sections( cfgs ), status::error );
}

}  // namespace servio::gov::tests
