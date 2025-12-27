
#include "../../drv/mock.hpp"
#include "../../gov/current/current.hpp"
#include "../../gov/position/position.hpp"
#include "../../gov/power/power.hpp"
#include "../../gov/velocity/velocity.hpp"
#include "../core.hpp"
#include "../dispatcher.hpp"

#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <emlabcpp/pmr/new_delete_resource.h>
#include <emlabcpp/range.h>
#include <git.h>
#include <gtest/gtest.h>
#include <random>
#include <source_location>

namespace servio::core::tests
{

TEST( core, dispatcher )
{
        drv::mock::pwm_mot mot;
        drv::mock::pos     gp;
        drv::mock::curr    gc;
        drv::mock::vcc     gv;
        drv::mock::temp    tm;
        drv::mock::stor    sd;

        cfg::map m{};
        core     cor{ 0_ms, gv, tm };

        dispatcher disp{
            .mem      = em::pmr::new_delete_resource(),
            .motor    = mot,
            .pos_drv  = gp,
            .curr_drv = gc,
            .vcc_drv  = gv,
            .temp_drv = tm,
            .gov      = cor.gov_,
            .met      = cor.met,
            .mon      = cor.mon,
            .cfg_map  = m,
            .stor_drv = sd,
            .conv     = cor.conv,
            .now      = 0_ms,
        };

        gov::create_governors( cor.gov_, cor.gov_mem );

        auto exec = [&]( std::string_view inpt ) {
                std::byte buff[666];
                auto [res, used] = handle_message(
                    disp, em::view_n( (std::byte const*) inpt.data(), inpt.size() ), buff );
                EXPECT_EQ( res, SUCCESS ) << inpt;
                std::string_view res_str( (char*) used.begin(), used.size() );
                return nlohmann::json::parse( res_str );
        };

        auto test = [&]( std::string_view     inpt,
                         nlohmann::json       j,
                         std::source_location loc = std::source_location::current() ) {
                auto res_j = exec( inpt );
                EXPECT_EQ( res_j, j ) << "inpt: " << inpt << "\n"
                                      << "at " << loc.file_name() << ":" << loc.line() << "\n";
        };

        // XXX: negative tests

        // mode
        auto tmp = cor.gov_.activate( "power", em::pmr::new_delete_resource() );
        EXPECT_EQ( tmp, SUCCESS );
        test( "govctl deactivate", R"(["OK"])"_json );
        EXPECT_EQ( cor.gov_.active(), nullptr );
        test( "govctl active", R"(["OK"])"_json );

        test( "govctl activate power", R"(["OK"])"_json );
        test( "gov power set 0.2", R"(["OK"])"_json );
        auto& p = dynamic_cast< gov::pow::_power_gov& >( *cor.gov_.active() );
        EXPECT_EQ( p.power, 0.2_pwr );
        test( "govctl active", R"(["OK", "power"])"_json );
        test( "govctl deactivate", R"(["OK"])"_json );

        test( "govctl activate current", R"(["OK"])"_json );
        test( "gov current set 0.5", R"(["OK"])"_json );
        auto& c = dynamic_cast< gov::curr::_current_gov& >( *cor.gov_.active() );
        EXPECT_EQ( c.goal, 0.5F );
        test( "govctl active", R"(["OK", "current"])"_json );
        test( "govctl deactivate", R"(["OK"])"_json );

        test( "govctl activate velocity", R"(["OK"])"_json );
        test( "gov velocity set 0.22", R"(["OK"])"_json );
        auto& v = dynamic_cast< gov::vel::_velocity_gov& >( *cor.gov_.active() );
        EXPECT_EQ( v.goal_vel, 0.22F );
        test( "govctl active", R"(["OK", "velocity"])"_json );
        test( "govctl deactivate", R"(["OK"])"_json );

        test( "govctl activate position", R"(["OK"])"_json );
        test( "gov position set -1", R"(["OK"])"_json );
        auto& pos = dynamic_cast< gov::pos::_position_gov& >( *cor.gov_.active() );
        EXPECT_EQ( pos.goal_pos, -1.0F );
        test( "govctl active", R"(["OK", "position"])"_json );
        test( "govctl deactivate", R"(["OK"])"_json );

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

        test( "cfg set model wololo", R"(["OK"])"_json );
        test( "cfg set current.curr_lim_min -0.4", R"(["OK"])"_json );
        test( "cfg get current.curr_lim_min", R"(["OK",-0.4])"_json );

        // cfg set/get
        for ( auto k : cfg::map::keys ) {

                auto res_j = exec( std::format( "cfg get {}", to_str( k ) ) );

                // verify get
                auto expected = m.ref_by_key( k );
                expected.visit(
                    [&]( vari::empty_t ) {
                            FAIL();
                    },
                    [&]< typename T >( T& x ) {
                            if constexpr ( std::same_as< T, cfg::encoder_mode > )
                                    EXPECT_EQ( res_j.at( 1 ), cfg::encoder_mode_to_str( x ) );
                            else if constexpr ( std::same_as< T, float > )
                                    EXPECT_NEAR( res_j.at( 1 ), x, 0.00001 );
                            else
                                    EXPECT_EQ( res_j.at( 1 ), x )
                                        << "key: " << to_str( k ) << " type: " << typeid( x ).name()
                                        << std::endl;
                    } );

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

                        std::string_view operator()( cfg::string )
                        {
                                return "wololo";
                        }
                };

                exec( std::format(
                    "cfg set {} {}",
                    to_str( k ),
                    expected.visit(
                        [&]( vari::empty_t ) -> std::string {
                                return "";
                        },
                        [&]( auto& x ) -> std::string {
                                return std::format( "{}", vary{}( x ) );
                        } ) ) );

                res_j = exec( std::format( "cfg get {}", to_str( k ) ) );

                // verify change
                expected = m.ref_by_key( k );
                expected.visit(
                    [&]( vari::empty_t ) {
                            FAIL();
                    },
                    [&]< typename T >( T& x ) {
                            if constexpr ( std::same_as< T, cfg::encoder_mode > )
                                    EXPECT_EQ( res_j.at( 1 ), cfg::encoder_mode_to_str( x ) );
                            else if constexpr ( std::same_as< T, float > )
                                    EXPECT_NEAR( res_j.at( 1 ), x, 0.00001 );
                            else
                                    EXPECT_EQ( res_j.at( 1 ), x )
                                        << "key: " << to_str( k ) << " type: " << typeid( x ).name()
                                        << std::endl;
                    } );
        }

        test( "cfg list 0", R"(["OK", "model"])"_json );
        test( "cfg list 2", R"(["OK", "group_id"])"_json );
        test( "cfg list 1024", R"(["OK"])"_json );

        EXPECT_EQ( sd.store_cnt, 0 );
        test( "cfg commit", R"(["OK"])"_json );
        EXPECT_EQ( sd.store_cnt, 1 );

        test( "cfg clear", R"(["OK"])"_json );
        EXPECT_EQ( sd.clear_cnt, 1 );
}

}  // namespace servio::core::tests
