
#include "../../cfg/default.hpp"
#include "../dispatcher.hpp"
#include "../map_cfg.hpp"

#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <emlabcpp/range.h>
#include <gtest/gtest.h>
#include <random>
#include <source_location>

namespace servio::core::tests
{
namespace mock
{
struct mot : drv::pwm_motor_iface
{
        void set_invert( bool ) override
        {
        }

        void set_power( pwr ) override
        {
        }

        void force_stop() override
        {
        }

        bool is_stopped() const override
        {
                return false;
        }

        int8_t get_direction() const override
        {
                return -1;
        };
};

struct get_pos : drv::get_pos_iface
{
        uint32_t x;

        uint32_t get_position() const override
        {
                return x;
        }

        limits< uint32_t > get_position_range() const override
        {
                return { 0, 1024 };
        }
};

struct get_curr : drv::get_curr_iface
{
        uint32_t x;

        uint32_t get_current() const override
        {
                return x;
        }
};

struct get_vcc_ : drv::vcc_iface
{
        uint32_t x;

        uint32_t get_vcc() const override
        {
                return x;
        }
};

struct get_temp : drv::temp_iface
{
        int32_t x;

        int32_t get_temperature() const override
        {
                return x;
        }
};

struct stor : drv::storage_iface
{
        std::size_t store_cnt = 0;

        em::result store_page( std::span< std::byte const > ) override
        {
                store_cnt += 1;
                return em::SUCCESS;
        }

        em::outcome load_page( std::span< std::byte > data ) override
        {
                std::ignore = data;
                return em::SUCCESS;
        }
};

}  // namespace mock

TEST( core, dispatcher )
{
        mock::mot      mot;
        mock::get_pos  gp;
        mock::get_curr gc;
        mock::get_vcc_ gv;
        mock::get_temp tm;
        mock::stor     sd;

        ctl::control ctl{
            0_ms,
            ctl::config{
                .position_limits = { -2.0F, 2.0F },
            } };
        mtr::metrics    mtr{ 0_ms, 0.0F, -1.0F, { -1.0F, 1.0F } };
        cfg::map        m = cfg::get_default_config();
        cfg::payload    pl;
        cnv::converter  cnv;
        mon::indication indi;
        mon::monitor    mon{ 0_ms, ctl, gv, tm, indi, cnv };

        cfg::dispatcher cdisp{
            .m     = m,
            .ctl   = ctl,
            .conv  = cnv,
            .met   = mtr,
            .mon   = mon,
            .motor = mot,
            .pos   = gp,
        };

        dispatcher disp{
            .motor    = mot,
            .pos_drv  = gp,
            .curr_drv = gc,
            .vcc_drv  = gv,
            .temp_drv = tm,
            .ctl      = ctl,
            .met      = mtr,
            .cfg_disp = cdisp,
            .cfg_pl   = pl,
            .stor_drv = sd,
            .conv     = cnv,
            .now      = 0_ms,
        };

        auto exec = [&]( std::string_view inpt ) {
                std::byte buff[42];
                auto [res, used] = handle_message(
                    disp, em::view_n( (std::byte const*) inpt.data(), inpt.size() ), buff );
                EXPECT_EQ( res, em::SUCCESS ) << inpt;
                std::string_view res_str( (char*) used.begin(), used.size() );
                return nlohmann::json::parse( res_str );
        };

        auto test = [&]( std::string_view inpt, nlohmann::json j ) {
                auto res_j = exec( inpt );
                EXPECT_EQ( res_j, j ) << "inpt: " << inpt << "\n";
        };

        // XXX: negative tests

        // mode
        ctl.switch_to_power_control( 0_pwr );
        test( "mode disengaged", R"(["OK"])"_json );
        EXPECT_EQ( ctl.get_mode(), control_mode::DISENGAGED );
        test( "prop mode", R"(["OK", "disengaged"])"_json );

        test( "mode power 0.2", R"(["OK"])"_json );
        EXPECT_EQ( ctl.get_power(), 0.2_pwr );
        test( "prop mode", R"(["OK", "power"])"_json );

        test( "mode current 0.5", R"(["OK"])"_json );
        EXPECT_EQ( ctl.get_desired_current(), 0.5F );
        test( "prop mode", R"(["OK", "current"])"_json );

        test( "mode velocity 0.22", R"(["OK"])"_json );
        EXPECT_EQ( ctl.get_desired_velocity(), 0.22F );
        test( "prop mode", R"(["OK", "velocity"])"_json );

        test( "mode position -1", R"(["OK"])"_json );
        EXPECT_EQ( ctl.get_desired_position(), -1.0F );
        test( "prop mode", R"(["OK", "position"])"_json );

        // prop
        gc.x = 42;
        test( "prop current", R"(["OK",-42.0])"_json );
        gv.x = 142;
        test( "prop vcc", R"(["OK",142.0])"_json );
        tm.x = 242;
        test( "prop temp", R"(["OK",242.0])"_json );
        gp.x = 342;
        test( "prop position", R"(["OK",342.0])"_json );

        test( "prop velocity", R"(["OK",-1.0])"_json );

        // cfg set/get
        std::tuple t = iface::field_tuple_t< iface::cfg >{};
        em::for_each( t, [&]< typename F >( F& ) {
                static constexpr std::string_view s = F::key.to_string();

                auto                      res_j = exec( std::format( "cfg get {}", s ) );
                static constexpr cfg::key k     = iface_to_cfg[iface::cfg_key{ F::key }.value()];

                // verify get
                if constexpr ( k == cfg::ENCODER_MODE ) {
                        auto expected =
                            ( m.get_val< k >() == cfg::ENC_MODE_QUAD ) ? "quad" : "analog";
                        EXPECT_EQ( res_j.at( 1 ), expected );
                } else if constexpr ( std::same_as< typename F::value_type, float > )
                        EXPECT_NEAR( res_j.at( 1 ), m.get_val< k >(), 0.00001 );
                else
                        EXPECT_EQ( res_j.at( 1 ), m.get_val< k >() )
                            << "key: " << F::key.to_string() << "\nk: " << k << std::endl;

                // vary
                struct vary
                {
                        float operator()( float x )
                        {
                                return -x;
                        }

                        uint32_t operator()( uint32_t x )
                        {
                                return x / 2;
                        }

                        bool operator()( bool x )
                        {
                                return !x;
                        }

                        std::string_view operator()( cfg::encoder_mode em )
                        {
                                return em == cfg::ENC_MODE_QUAD ? "analog" : "quad";
                        }

                        std::string_view operator()( cfg::model_name )
                        {
                                return "wololo";
                        }
                };

                exec( std::format( "cfg set {} {}", s, vary{}( m.get_val< k >() ) ) );

                res_j = exec( std::format( "cfg get {}", s ) );

                // verify change
                if constexpr ( k == cfg::ENCODER_MODE ) {
                        auto expected =
                            ( m.get_val< k >() == cfg::ENC_MODE_QUAD ) ? "quad" : "analog";
                        EXPECT_EQ( res_j.at( 1 ), expected );
                } else if constexpr ( std::same_as< typename F::value_type, float > )
                        EXPECT_NEAR( res_j.at( 1 ), m.get_val< k >(), 0.00001 );
                else
                        EXPECT_EQ( res_j.at( 1 ), m.get_val< k >() )
                            << "key: " << F::key.to_string() << "\nk: " << k << std::endl;
        } );

        EXPECT_EQ( sd.store_cnt, 0 );
        test( "cfg commit", R"(["OK"])"_json );
        EXPECT_EQ( sd.store_cnt, 1 );
        test( "cfg clear", R"(["OK"])"_json );
        EXPECT_EQ( sd.store_cnt, 2 );
}

}  // namespace servio::core::tests
