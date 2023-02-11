#include "control.hpp"
#include "motor.hpp"

#include <emlabcpp/range.h>
#include <gtest/gtest.h>

class ControlFixture : public ::testing::Test
{
public:
        void SetUp()
        {
                now = 0ms;
                motor.emplace();
                ctl.emplace(
                    now,
                    ctl::config{
                        .position_pid{
                            .p = 4.f,
                            .i = 0.f,
                            .d = 0.f,
                        },
                        .velocity_pid{
                            .p = 16.f,
                            .i = 0.0005f,
                            .d = 4.f,
                        },
                        .current_pid{
                            .p = 2048.f,
                            .i = 512.f,
                            .d = 1024.f,
                        },
                        .position_limits{
                            -10.f,
                            10.f,
                        },
                        .velocity_limits{
                            -10.f,
                            10.f,
                        },
                        .current_limits{
                            -10.f,
                            10.f,
                        },
                    } );
        }

        void tick()
        {
                int16_t power = ctl->get_power();

                motor->apply_power( now, power );

                ctl->position_irq( now, motor->position() );
                ctl->velocity_irq( now, motor->velocity );
                ctl->current_irq( now, motor->current );
        }

        std::chrono::milliseconds         now = 0ms;
        std::optional< simple_motor_sim > motor;
        std::optional< control >          ctl;
};

TEST_F( ControlFixture, current )
{

        for ( float curr : { 1.f, 2.f, -1.f, 0.f, 1.f } ) {
                ctl->switch_to_current_control( now, curr );

                for ( std::size_t i : em::range( 100u ) ) {
                        std::ignore = i;
                        tick();
                        now += 5ms;
                }

                EXPECT_NEAR( motor->current, curr, 0.005f );
        }
}

TEST_F( ControlFixture, velocity )
{

        for ( float vel : { 1.f, 2.f, -1.f, 0.f, 1.f } ) {
                ctl->switch_to_velocity_control( now, vel );

                for ( std::size_t i : em::range( 200u ) ) {
                        std::ignore = i;
                        std::cout << *motor << std::endl;
                        tick();
                        now += 5ms;
                }

                EXPECT_NEAR( motor->velocity, vel, 0.015f );
        }
}

TEST_F( ControlFixture, position )
{

        for ( float angle : { 1.f, 2.f, 1.f, -1.f } ) {
                ctl->switch_to_position_control( now, angle );

                for ( std::size_t i : em::range( 500u ) ) {
                        std::ignore = i;
                        std::cout << *motor << std::endl;
                        tick();
                        now += 5ms;
                }

                EXPECT_NEAR( motor->position(), angle, 0.015f );
        }
}
