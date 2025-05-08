
#include "../../drv/mock.hpp"
#include "../dispatcher.hpp"

#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <emlabcpp/range.h>
#include <git.h>
#include <gtest/gtest.h>
#include <random>
#include <source_location>

namespace servio::core::tests
{

using namespace avakar::literals;

TEST( core, dispatcher )
{
        drv::mock::pwm_mot mot;
        drv::mock::pos     gp;
        drv::mock::curr    gc;
        drv::mock::vcc     gv;
        drv::mock::temp    tm;
        drv::mock::stor    sd;

        cfg::map m{};
        core     cor{
            0_ms,
            gv,
            tm,
            ctl::config{
                    .position_limits = { -2.0F, 2.0F },
            } };

        dispatcher disp{
            .motor    = mot,
            .pos_drv  = gp,
            .curr_drv = gc,
            .vcc_drv  = gv,
            .temp_drv = tm,
            .ctl      = cor.ctl,
            .met      = cor.met,
            .mon      = cor.mon,
            .cfg_map  = m,
            .stor_drv = sd,
            .conv     = cor.conv,
            .now      = 0_ms,
        };

        auto exec = [&]( std::string_view inpt ) {
                std::byte buff[666];
                auto [res, used] = handle_message(
                    disp, em::view_n( (std::byte const*) inpt.data(), inpt.size() ), buff );
                EXPECT_EQ( res, SUCCESS ) << inpt;
                std::string_view res_str( (char*) used.begin(), used.size() );
                return nlohmann::json::parse( res_str );
        };

        auto test = [&]( std::string_view inpt, nlohmann::json j ) {
                auto res_j = exec( inpt );
                EXPECT_EQ( res_j, j ) << "inpt: " << inpt << "\n";
        };

        // XXX: negative tests

        // mode
        cor.ctl.switch_to_power_control( 0_pwr );
        test( "mode disengaged", R"(["OK"])"_json );
        EXPECT_EQ( cor.ctl.get_mode(), control_mode::DISENGAGED );
        test( "prop mode", R"(["OK", "disengaged"])"_json );

        test( "mode power 0.2", R"(["OK"])"_json );
        EXPECT_EQ( cor.ctl.get_power(), 0.2_pwr );
        test( "prop mode", R"(["OK", "power"])"_json );

        test( "mode current 0.5", R"(["OK"])"_json );
        EXPECT_EQ( cor.ctl.get_desired_current(), 0.5F );
        test( "prop mode", R"(["OK", "current"])"_json );

        test( "mode velocity 0.22", R"(["OK"])"_json );
        EXPECT_EQ( cor.ctl.get_desired_velocity(), 0.22F );
        test( "prop mode", R"(["OK", "velocity"])"_json );

        test( "mode position -1", R"(["OK"])"_json );
        EXPECT_EQ( cor.ctl.get_desired_position(), -1.0F );
        test( "prop mode", R"(["OK", "position"])"_json );

        test(
            "info",
            nlohmann::json::parse( std::format(
                R"(["OK", {{"commit":"{}","version":"{}" }}])",
                git::CommitSHA1(),
                git::Describe() ) ) );

        // prop
        gc.x = 42;
        test( "prop current", R"(["OK",-42.0])"_json );
        gv.x = 142;
        test( "prop vcc", R"(["OK",142.0])"_json );
        tm.x = 242;
        test( "prop temp", R"(["OK",242.0])"_json );
        gp.x = 342;
        test( "prop position", R"(["OK",342.0])"_json );

        test( "prop velocity", R"(["OK",0.0])"_json );

        // cfg set/get
        std::tuple t = iface::field_tuple_t< iface::cfg >{};
        em::for_each( t, [&]< typename F >( F& ) {
                static constexpr std::string_view s  = F::key.to_string();
                static constexpr cfg::key         kv = *cfg::str_to_key( s );

                auto res_j = exec( std::format( "cfg get {}", s ) );

                // verify get
                if constexpr ( iface::cfg_key{ F::key } == "encoder_mode"_a ) {
                        auto expected = m.ref_by_key< kv >();
                        EXPECT_EQ( res_j.at( 1 ), cfg::encoder_mode_to_str( expected ) );
                } else if constexpr ( std::same_as< typename F::value_type, float > )
                        EXPECT_NEAR( res_j.at( 1 ), m.ref_by_key< kv >(), 0.00001 );
                else
                        EXPECT_EQ( res_j.at( 1 ), m.ref_by_key< kv >() )
                            << "key: " << F::key.to_string() << "\nk: " << F::key.to_string()
                            << std::endl;

                // vary
                struct vary
                {
                        float operator()( float x )
                        {
                                if ( x < 0.0001F )
                                        return 1.0F;
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
                                return em == cfg::encoder_mode::quad ? "analog" : "quad";
                        }

                        std::string_view operator()( cfg::string_type )
                        {
                                return "wololo";
                        }
                };

                exec( std::format( "cfg set {} {}", s, vary{}( m.ref_by_key< kv >() ) ) );

                res_j = exec( std::format( "cfg get {}", s ) );

                // verify change
                if constexpr ( iface::cfg_key{ F::key } == "encoder_mode"_a ) {
                        auto expected = m.ref_by_key< kv >();
                        EXPECT_EQ( res_j.at( 1 ), cfg::encoder_mode_to_str( expected ) );
                } else if constexpr ( std::same_as< typename F::value_type, float > )
                        EXPECT_NEAR( res_j.at( 1 ), m.ref_by_key< kv >(), 0.00001 );
                else
                        EXPECT_EQ( res_j.at( 1 ), m.ref_by_key< kv >() )
                            << "key: " << F::key.to_string() << "\nk: " << F::key.to_string()
                            << std::endl;
        } );

        EXPECT_EQ( sd.store_cnt, 0 );
        test( "cfg commit", R"(["OK"])"_json );
        EXPECT_EQ( sd.store_cnt, 1 );

        test( "cfg clear", R"(["OK"])"_json );
        EXPECT_EQ( sd.clear_cnt, 1 );
}

}  // namespace servio::core::tests
