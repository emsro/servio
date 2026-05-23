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

struct current_ctl_test : utest
{
        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pwm_motor_iface& motor;

        core::core& cor;

        char const* name = "current_ctl_test";

        asrt::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = setup_poweroff( cor.gov_ );

                float expected = 0.2F;
                std::ignore    = cor.gov_.activate( "current", cor.gov_mem );
                auto* p        = dynamic_cast< gov::curr::_current_gov* >( cor.gov_.active() );
                co_await expect( *this, p != nullptr );
                p->set_goal_current( expected );

                float                        avg_curr = 0;
                static constexpr std::size_t count    = 10000000;

                drv::wait_for( clk, 350_ms );
                for ( std::size_t i : em::range( count ) ) {
                        std::ignore   = i;
                        float current = cnv::current( cor.conv, curr, motor );
                        avg_curr += current;
                }
                avg_curr /= static_cast< float >( count );

                co_await store_metric( *this, "measured", avg_curr );
                co_await store_metric( *this, "expected", expected );
                co_await expect( *this, em::almost_equal( avg_curr, expected, 0.1F ) );
        }
};

struct sign_test : utest
{
        drv::clk_iface&       clk;
        drv::curr_iface&      curr;
        drv::pos_iface&       pos;
        drv::pwm_motor_iface& motor;

        core::core& cor;

        char const* name = "sign_test";

        asrt::task< void > exec()
        {
                co_await init_utest( *this );

                em::defer d = setup_poweroff( cor.gov_ );
                rewind( cor, clk, pos, 250_ms, { 0.0f, 0.3f }, 0.5f, [] {} );

                std::ignore = cor.gov_.activate( "power", cor.gov_mem );
                auto* p     = dynamic_cast< gov::pow::_power_gov* >( cor.gov_.active() );
                co_await expect( *this, p != nullptr );
                p->power = p_low / 2.F;
                drv::wait_for( clk, 200_ms );
                std::size_t count = 50;

                float csum = 0;
                float vsum = 0;

                for ( std::size_t i : em::range( count ) ) {
                        std::ignore   = i;
                        float current = servio::cnv::current( cor.conv, curr, motor );
                        float vel     = cor.met.get_velocity();
                        csum += current;
                        vsum += vel;
                }

                co_await store_metric( *this, "csum", csum );
                co_await store_metric( *this, "vsum", vsum );
                co_await expect( *this, std::signbit( csum ) == std::signbit( vsum ) );
                co_await expect( *this, cor.gov_.deactivate() == status::success );
        }
};

inline void setup_ctl_test(
    asrt::task_ctx&       ctx,
    asrt_reac_assm&       assm,
    drv::clk_iface&       clk,
    drv::pwm_motor_iface& motor,
    drv::curr_iface&      curr,
    drv::pos_iface&       pos,
    core::core&           cor,
    status&               res )
{
        setup_utest( ctx, assm, res, current_ctl_test{ { ctx, assm }, clk, curr, motor, cor } );
        setup_utest( ctx, assm, res, sign_test{ { ctx, assm }, clk, curr, pos, motor, cor } );
}

}  // namespace servio::ftest::intg
