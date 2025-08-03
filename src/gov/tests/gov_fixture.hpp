#pragma once

#include "../../sim/motor.hpp"
#include "../governor.hpp"

#include <gtest/gtest.h>
#include <iostream>

namespace servio::gov
{

struct gov_fixture : public ::testing::Test
{
        sim::simple_motor motor;

        gov_fixture()
          : motor()
        {
                motor.static_friction_force = 0.F;
        }

        void tick( handle& gov, microseconds now )
        {

                gov.metrics_irq( now, motor.current, motor.velocity, true );
                pwr p = gov.current_irq( now, motor.current );
                motor.apply_power( now, p );
                std::cout << "time: " << now.count() << "ms, "
                          << "power: " << p << ", "
                          << "curr: " << motor.current << ", "
                          << "vel: " << motor.velocity << ", "
                          << "pos: " << motor.position() << "\n";
        }
};

}  // namespace servio::gov
