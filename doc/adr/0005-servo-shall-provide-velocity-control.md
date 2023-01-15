# 5. Servo shall provide velocity control

Date: 2023-01-15

## Status

Accepted

Parent of [7. Chained control loops architecture shall be used](0007-chained-control-loops-architecture-shall-be-used.md)

## Context

The project represents firmware for servomotors, these however have various forms of modes.
That is, what exactly the input specifies and what is the servo trying to achieve.

## Decision

We agreed to provide angular velocity control.
In this mode, the servomotor gets desired angular velocity as an input.

The system assumes that the motor output shaft is connected in appropiate manner to the currently used position sensor.
The system should be able to provide estimates of current angular velocity based on the mentioned position sensor.
Velocity control shall provide bi-directional control of the motor, so that the shaft angular velocity remains at desired value.

The input is in radians per second.

## Consequences

Given the decision, velocity control is one of the modes that has to be designed, tested, and verified that it works.

