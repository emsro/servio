#pragma once

#include "../../cnv/utils.hpp"
#include "../../core/core.hpp"
#include "../../drv/callbacks.hpp"
#include "../../drv/interfaces.hpp"
#include "../../drv/retainers.hpp"
#include "../utest.hpp"

namespace servio::ftest::intg
{

struct meas_cur_test : utest
{
        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;
        core::core&           cor;

        char const* name = "measure_current";

        asrt::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = setup_poweroff( cor.gov_ );

                em::defer             dc = drv::retain_callback( curr );
                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                motor.set_power( p_max );

                static constexpr std::size_t iters = 50;
                float                        sum   = 0;
                for ( std::size_t i = 0; i < iters; i++ ) {
                        float current = cnv::current( cor.conv, curr, motor );
                        sum += current;
                        drv::wait_for( clk, 10_ms );
                }
                co_await expect( *this, !em::almost_equal( sum / iters, 0.f, 0.01f ) );
                co_await store_metric( *this, "avg_current", sum / iters );
                motor.set_power( p_max );
        }
};

struct meas_pos_test : utest
{
        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;
        core::core&           cor;

        char const* name = "measure_position";

        asrt::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = setup_poweroff( cor.gov_ );

                em::defer             dc = drv::retain_callback( curr );
                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                motor.set_power( p_max / 2.F );
                float start = cor.met.get_position();

                drv::wait_for( clk, 400_us );

                float end = cor.met.get_position();
                motor.set_power( 0_pwr );

                float expected_angle_change = 0.F;
                co_await expect( *this, std::abs( start - end ) > expected_angle_change );
                co_await store_metric( *this, "pos1", start );
                co_await store_metric( *this, "pos2", end );
        }
};

struct meas_vel_test : utest
{
        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;
        core::core&           cor;

        char const* name = "measure_vel";

        asrt::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = setup_poweroff( cor.gov_ );

                em::defer             dc = drv::retain_callback( curr );
                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                float       sum          = 0.f;
                std::size_t measurements = 1024u;

                motor.set_power( p_max / 2.F );

                for ( std::size_t i = 0; i < measurements; i++ ) {
                        float vel = cor.met.get_velocity();
                        sum += vel;
                }

                motor.set_power( 0_pwr );

                float average_velocity = sum / static_cast< float >( measurements );
                co_await expect( *this, !em::almost_equal( average_velocity, 0.f, 0.01f ) );
                co_await store_metric( *this, "average velocity", average_velocity );
        }
};

inline void setup_meas_tests(
    asrt::task_ctx&       ctx,
    asrt_reac_assm&       assm,
    drv::clk_iface&       clk,
    drv::pwm_motor_iface& motor,
    drv::curr_iface&      curr,
    core::core&           cor,
    status&               res )
{
        setup_utest( ctx, assm, res, meas_vel_test{ { ctx, assm }, clk, curr, motor, cor } );
        setup_utest( ctx, assm, res, meas_pos_test{ { ctx, assm }, clk, curr, motor, cor } );
        setup_utest( ctx, assm, res, meas_cur_test{ { ctx, assm }, clk, curr, motor, cor } );
}

}  // namespace servio::ftest::intg
