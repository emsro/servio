#pragma once

#include "cnv/utils.hpp"
#include "core/core.hpp"
#include "drv/callbacks.hpp"
#include "drv/interfaces.hpp"
#include "drv/retainers.hpp"
#include "ftest/is_powerless.hpp"
#include "ftest/rewind.hpp"
#include "ftest/utest.hpp"

namespace servio::ftest::tests
{

struct current_ctl_test
{
        t::parameters& params;

        drv::clk_interface&       clk;
        drv::current_interface&   curr;
        drv::pwm_motor_interface& motor;

        core::core& cor;

        std::string_view name = "ctl_test";

        t::coroutine< void > run( auto&, collector& coll )
        {
                if ( co_await is_powerless( params ) )
                        co_await t::skip();
                setup_poweroff( cor.ctl );

                float expected = 0.2F;
                cor.ctl.switch_to_current_control( clk.get_us(), expected );

                float                        avg_curr = 0;
                static constexpr std::size_t count    = 10000;

                drv::wait_for( clk, 200_us );
                for ( std::size_t i : em::range( count ) ) {
                        std::ignore   = i;
                        float current = cnv::current( cor.conv, curr, motor );
                        avg_curr += current;
                }
                avg_curr /= static_cast< float >( count );

                coll->set( "measured", avg_curr );
                coll->set( "expected", expected );
                co_await t::expect( em::almost_equal( avg_curr, expected, 0.1F ) );
        }
};

struct sign_test
{
        t::parameters& params;

        drv::clk_interface&       clk;
        drv::current_interface&   curr;
        drv::position_interface&  pos;
        drv::pwm_motor_interface& motor;

        core::core& cor;

        std::string_view name = "sign_test";

        t::coroutine< void > run( auto&, collector& coll )
        {
                if ( co_await is_powerless( params ) )
                        co_await t::skip();
                setup_poweroff( cor.ctl );
                rewind( cor, clk, pos, 250_ms, { 0.0f, 0.3f }, 0.2f );

                t::node_id did = co_await coll->set( "data", em::contiguous_container_type::ARRAY );

                cor.ctl.switch_to_current_control( clk.get_us(), 0.3f );
                std::size_t count = 50;
                for ( std::size_t i : em::range( count ) ) {
                        std::ignore = i;
                        t::node_id nid =
                            co_await coll->append( did, em::contiguous_container_type::OBJECT );

                        float current = servio::cnv::current( cor.conv, curr, motor );
                        float pos     = cor.met.get_position();
                        float vel     = cor.met.get_velocity();

                        coll->set( nid, "now", clk.get_us() );
                        coll->set( nid, "cur", current );
                        coll->set( nid, "pos", pos );
                        coll->set( nid, "vel", vel );

                        co_await t::expect( std::signbit( current ) == std::signbit( vel ) );
                }
                cor.ctl.switch_to_current_control( clk.get_us(), 0.0f );
                motor.set_power( 0 );
        }
};

inline void setup_ctl_test(
    em::pmr::memory_resource& mem,
    t::reactor&               reac,
    t::collector&             coll,
    t::parameters&            params,
    drv::clk_interface&       clk,
    drv::pwm_motor_interface& motor,
    drv::current_interface&   curr,
    drv::position_interface&  pos,
    core::core&               cor,
    em::result&               res )
{
        setup_utest< current_ctl_test >( mem, reac, coll, res, params, clk, curr, motor, cor );
        setup_utest< sign_test >( mem, reac, coll, res, params, clk, curr, pos, motor, cor );
}

}  // namespace servio::ftest::tests
