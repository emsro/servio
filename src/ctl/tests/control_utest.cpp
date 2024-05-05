#include "ctl/control.hpp"
#include "sim/motor.hpp"

#include <emlabcpp/experimental/logging.h>
#include <emlabcpp/experimental/logging/util.h>
#include <emlabcpp/range.h>
#include <gtest/gtest.h>

namespace servio::ctl::tests
{

class control_fixture : public ::testing::Test
{
public:
        void SetUp() override
        {
                now = 0_ms;
                motor.emplace();
                motor->static_friction_force = 0.F;
                ctl.emplace(
                    now,
                    config{
                        .position_pid{
                            .p = 0.03F,
                            .i = 0.0F,
                            .d = 0.05F,
                        },
                        .velocity_pid{
                            .p = 16.F,
                            .i = 0.0005F,
                            .d = 4.F,
                        },
                        .current_pid{
                            .p = 0.0625F,
                            .i = 0.000'005'12F,
                            .d = 0.03125F,
                        },
                        .position_limits{
                            -10.F,
                            10.F,
                        },
                        .velocity_limits{
                            -10.F,
                            10.F,
                        },
                        .current_limits{
                            -3.F,
                            3.F,
                        },
                        .static_friction_scale = 1.F,
                        .static_friction_decay = 1.F,
                    } );
        }

        void tick()
        {
                pwr power = ctl->get_power();

                motor->apply_power( now, power );

                ctl->position_irq( now, motor->position() );
                ctl->velocity_irq( now, motor->velocity );
                ctl->current_irq( now, motor->current );

                EMLABCPP_INFO_LOG_VARS(
                    *power, motor->position(), motor->velocity, ctl->get_desired_current() );
        }

        microseconds                       now = 0_ms;
        std::optional< sim::simple_motor > motor;
        std::optional< ctl::control >      ctl;
};

TEST_F( control_fixture, current )
{

        for ( float curr : { 0.1F, 0.2F, -0.1F, 0.F, 0.1F } ) {
                ctl->switch_to_current_control( now, curr );

                for ( std::size_t i : em::range( 100u ) ) {
                        std::ignore = i;
                        tick();
                        now += 5_ms;
                }

                EXPECT_NEAR( motor->current, curr, 0.005F );
        }
}

TEST_F( control_fixture, velocity )
{

        for ( float vel : { 1.F, 2.F, -1.F, 0.F, 1.F } ) {
                ctl->switch_to_velocity_control( now, vel );

                for ( std::size_t i : em::range( 200u ) ) {
                        std::ignore = i;
                        tick();
                        now += 5_ms;
                }

                EXPECT_NEAR( motor->velocity, vel, 0.015F );
        }
}

// TODO: eeeeh, this should be fixed
TEST_F( control_fixture, DISABLED_position )
{

        for ( float angle : { 1.F, 2.F, 1.F, -1.F } ) {
                ctl->switch_to_position_control( now, angle );

                for ( std::size_t i : em::range( 700u ) ) {
                        std::ignore = i;
                        tick();
                        now += 5_ms;
                }

                EXPECT_NEAR( motor->position(), angle, 0.015F );
        }
}

}  // namespace servio::ctl::tests
