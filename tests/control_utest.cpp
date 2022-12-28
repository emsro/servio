#include "control.hpp"
#include "motor.hpp"

#include <emlabcpp/range.h>
#include <gtest/gtest.h>

using namespace std::literals::chrono_literals;

class ControlFixture : public ::testing::Test
{
public:
    void SetUp()
    {
        now = 0ms;
        motor.emplace();
        ctl.emplace( now );

        ctl->set_pid( control_loop::CURRENT, 2048.f, 512.f, 1024.f );
        ctl->set_pid( control_loop::VELOCITY, 16.0f, 0.0005f, 4.f );
        ctl->set_pid( control_loop::POSITION, 4.0f, 0.f, 0.f );
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
