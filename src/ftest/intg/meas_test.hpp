#pragma once

#include "../../cnv/utils.hpp"
#include "../../core/core.hpp"
#include "../../drv/callbacks.hpp"
#include "../../drv/interfaces.hpp"
#include "../../drv/retainers.hpp"
#include "../rewind.hpp"
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

        ftest::task< void > exec()
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
                        co_await wait_for( *this, clk, 10_ms );
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

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = setup_poweroff( cor.gov_ );

                em::defer             dc = drv::retain_callback( curr );
                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                motor.set_power( p_max / 2.F );
                float        start      = cor.met.get_position();
                auto         stream     = co_await asrt::define< float, float >( assm.stream, 0 );
                microseconds stream_end = clk.get_us() + 10_ms;
                while ( clk.get_us() < stream_end ) {
                        float t_ms     = static_cast< float >( clk.get_us().count() ) / 1000.0F;
                        float position = cor.met.get_position();
                        co_await asrt::emit( stream, t_ms, position );
                }

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
        drv::pos_iface&       pos;
        drv::pwm_motor_iface& motor;
        core::core&           cor;

        char const* name = "measure_vel";

        ftest::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = setup_poweroff( cor.gov_ );

                em::defer             dc = drv::retain_callback( curr );
                drv::empty_current_cb ccb;
                curr.set_current_callback( ccb );

                // --- Pre-rewind: capture raw position for 20 ms ---
                {
                        auto stream = co_await asrt::define< float, uint32_t >( assm.stream, 0 );
                        microseconds stream_end = clk.get_us() + 20_ms;
                        while ( clk.get_us() < stream_end ) {
                                float t_ms = static_cast< float >( clk.get_us().count() ) / 1000.0F;
                                uint32_t raw_pos = pos.get_position();
                                co_await asrt::emit( stream, t_ms, raw_pos );
                        }
                }

                rewind( cor, clk, pos, 250_ms, { 3.0f, 3.3f }, 0.5f, [] {} );

                // --- Post-rewind: capture raw position for 20 ms ---
                {
                        auto stream = co_await asrt::define< float, uint32_t >( assm.stream, 0 );
                        microseconds stream_end = clk.get_us() + 20_ms;
                        while ( clk.get_us() < stream_end ) {
                                float t_ms = static_cast< float >( clk.get_us().count() ) / 1000.0F;
                                uint32_t raw_pos = pos.get_position();
                                co_await asrt::emit( stream, t_ms, raw_pos );
                        }
                }

                float    sum            = 0.f;
                uint32_t stream_samples = 0;

                motor.set_power( p_max / 2.F );
                co_await wait_for( *this, clk, 100_ms );

                // --- After warmup: capture raw position for 20 ms ---
                {
                        auto stream = co_await asrt::define< float, uint32_t >( assm.stream, 0 );
                        microseconds stream_end = clk.get_us() + 20_ms;
                        while ( clk.get_us() < stream_end ) {
                                float t_ms = static_cast< float >( clk.get_us().count() ) / 1000.0F;
                                uint32_t raw_pos = pos.get_position();
                                co_await asrt::emit( stream, t_ms, raw_pos );
                        }
                }

                auto stream = co_await asrt::define< float, float, float, float, uint32_t, bool >(
                    assm.stream, 0 );
                microseconds stream_end = clk.get_us() + 500_ms;
                while ( clk.get_us() < stream_end ) {
                        float    t_ms     = static_cast< float >( clk.get_us().count() ) / 1000.0F;
                        float    vel      = cor.met.get_velocity();
                        float    current  = cnv::current( cor.conv, curr, motor );
                        float    position = cor.met.get_position();
                        uint32_t raw_pos  = pos.get_position();
                        bool     same_signs = std::signbit( current ) == std::signbit( vel );
                        co_await asrt::emit(
                            stream, t_ms, vel, current, position, raw_pos, same_signs );
                        sum += vel;
                        stream_samples += 1;
                }

                motor.set_power( 0_pwr );

                float average_velocity = 0.f;
                if ( stream_samples != 0 )
                        average_velocity = sum / static_cast< float >( stream_samples );

                co_await store_metric( *this, "average velocity", average_velocity );
                co_await expect( *this, !em::almost_equal( average_velocity, 0.f, 0.001f ) );
        }
};

inline void setup_meas_tests(
    asrt::task_ctx&       ctx,
    asrt_reac_assm&       assm,
    drv::clk_iface&       clk,
    drv::pos_iface&       pos,
    drv::pwm_motor_iface& motor,
    drv::curr_iface&      curr,
    core::core&           cor,
    status&               res )
{
        setup_utest( ctx, assm, res, meas_vel_test{ { ctx, assm }, clk, curr, pos, motor, cor } );
        setup_utest( ctx, assm, res, meas_pos_test{ { ctx, assm }, clk, curr, motor, cor } );
        setup_utest( ctx, assm, res, meas_cur_test{ { ctx, assm }, clk, curr, motor, cor } );
}

}  // namespace servio::ftest::intg
